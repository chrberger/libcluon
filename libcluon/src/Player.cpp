/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/Player.hpp"
#include "cluon/Envelope.hpp"
#include "cluon/Time.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <thread>
#include <utility>

namespace cluon {

IndexEntry::IndexEntry(const int64_t &sampleTimeStamp, const uint64_t &filePosition) noexcept
    : m_sampleTimeStamp(sampleTimeStamp)
    , m_filePosition(filePosition)
    , m_available(false) {}

////////////////////////////////////////////////////////////////////////

Player::Player(const std::string &file, const bool &autoRewind, const bool &threading) noexcept
    : m_threading(threading)
    , m_file(file)
    , m_recFile()
    , m_recFileValid(false)
    , m_autoRewind(autoRewind)
    , m_indexMutex()
    , m_index()
    , m_previousPreviousEnvelopeAlreadyReplayed(m_index.end())
    , m_previousEnvelopeAlreadyReplayed(m_index.begin())
    , m_currentEnvelopeToReplay(m_index.begin())
    , m_nextEntryToReadFromRecFile(m_index.begin())
    , m_desiredInitialLevel(0)
    , m_firstTimePointReturningAEnvelope()
    , m_numberOfReturnedEnvelopesInTotal(0)
    , m_delay(0)
    , m_envelopeCacheFillingThreadIsRunningMutex()
    , m_envelopeCacheFillingThreadIsRunning(false)
    , m_envelopeCacheFillingThread()
    , m_envelopeCache()
    , m_playerListenerMutex()
    , m_playerListener(nullptr) {
    initializeIndex();
    computeInitialCacheLevelAndFillCache();

    if (m_threading) {
        // Start concurrent thread to manage cache.
        setEnvelopeCacheFillingRunning(true);
        m_envelopeCacheFillingThread = std::thread(&Player::manageCache, this);
    }
}

Player::~Player() {
    if (m_threading) {
        // Stop concurrent thread to manage cache.
        setEnvelopeCacheFillingRunning(false);
        m_envelopeCacheFillingThread.join();
    }

    m_recFile.close();
}

////////////////////////////////////////////////////////////////////////

void Player::setPlayerListener(std::function<void(cluon::data::PlayerStatus playerStatus)> playerListener) noexcept {
    std::lock_guard<std::mutex> lck(m_playerListenerMutex);
    m_playerListener = playerListener;
}

////////////////////////////////////////////////////////////////////////

void Player::initializeIndex() noexcept {
    m_recFile.open(m_file.c_str(), std::ios_base::in | std::ios_base::binary); /* Flawfinder: ignore */
    m_recFileValid = m_recFile.good();

    if (m_recFileValid) {
        // Determine file size to display progress.
        m_recFile.seekg(0, m_recFile.end);
        int64_t fileLength = m_recFile.tellg();
        m_recFile.seekg(0, m_recFile.beg);

        // Read complete file and store file positions to envelopes to create
        // index of available data. The actual reading of Envelopes is deferred.
        uint64_t totalBytesRead = 0;
        const cluon::data::TimeStamp BEFORE{cluon::time::now()};
        {
            int32_t oldPercentage = -1;
            while (m_recFile.good()) {
                const uint64_t POS_BEFORE = static_cast<uint64_t>(m_recFile.tellg());
                auto retVal               = extractEnvelope(m_recFile);
                const uint64_t POS_AFTER  = static_cast<uint64_t>(m_recFile.tellg());

                if (!m_recFile.eof() && retVal.first) {
                    totalBytesRead += (POS_AFTER - POS_BEFORE);

                    // Store mapping .rec file position --> index entry.
                    const int64_t microseconds = cluon::time::toMicroseconds(retVal.second.sampleTimeStamp());
                    m_index.emplace(std::make_pair(microseconds, IndexEntry(microseconds, POS_BEFORE)));

                    const int32_t percentage = static_cast<int32_t>((static_cast<float>(m_recFile.tellg()) * 100.0f) / static_cast<float>(fileLength));
                    if ((percentage % 5 == 0) && (percentage != oldPercentage)) {
                        std::clog << "[cluon::Player]: Indexed " << percentage << "% from " << m_file << "." << std::endl;
                        oldPercentage = percentage;
                    }
                }
            }
        }
        const cluon::data::TimeStamp AFTER{cluon::time::now()};

        std::clog << "[cluon::Player]: " << m_file << " contains " << m_index.size() << " entries; "
                  << "read " << totalBytesRead << " bytes "
                  << "in " << cluon::time::deltaInMicroseconds(AFTER, BEFORE) / static_cast<int64_t>(1000 * 1000) << "s." << std::endl;
    } else {
        std::clog << "[cluon::Player]: " << m_file << " could not be opened." << std::endl;
    }
}

void Player::resetCaches() noexcept {
    try {
        std::lock_guard<std::mutex> lck(m_indexMutex);
        m_delay                            = 0;
        m_numberOfReturnedEnvelopesInTotal = 0;
        m_envelopeCache.clear();
    } catch (...) {} // LCOV_EXCL_LINE
}

void Player::resetIterators() noexcept {
    try {
        std::lock_guard<std::mutex> lck(m_indexMutex);
        // Point to first entry in index.
        m_nextEntryToReadFromRecFile = m_previousEnvelopeAlreadyReplayed = m_currentEnvelopeToReplay = m_index.begin();
        // Invalidate iterator for erasing entries point.
        m_previousPreviousEnvelopeAlreadyReplayed = m_index.end();
    } catch (...) {} // LCOV_EXCL_LINE
}

void Player::computeInitialCacheLevelAndFillCache() noexcept {
    if (m_recFileValid && (m_index.size() > 0)) {
        int64_t smallestSampleTimePoint = std::numeric_limits<int64_t>::max();
        int64_t largestSampleTimePoint  = std::numeric_limits<int64_t>::min();
        for (auto it = m_index.begin(); it != m_index.end(); it++) {
            smallestSampleTimePoint = std::min(smallestSampleTimePoint, it->first);
            largestSampleTimePoint  = std::max(largestSampleTimePoint, it->first);
        }

        const uint32_t ENTRIES_TO_READ_PER_SECOND_FOR_REALTIME_REPLAY
            = static_cast<uint32_t>(std::ceil(static_cast<float>(m_index.size()) * (static_cast<float>(Player::ONE_SECOND_IN_MICROSECONDS))
                                              / static_cast<float>(largestSampleTimePoint - smallestSampleTimePoint)));
        m_desiredInitialLevel = std::max<uint32_t>(ENTRIES_TO_READ_PER_SECOND_FOR_REALTIME_REPLAY * Player::LOOK_AHEAD_IN_S, MIN_ENTRIES_FOR_LOOK_AHEAD);

        std::clog << "[cluon::Player]: Initializing cache with " << m_desiredInitialLevel << " entries." << std::endl;

        resetCaches();
        resetIterators();
        fillEnvelopeCache(m_desiredInitialLevel);
    }
}

uint32_t Player::fillEnvelopeCache(const uint32_t &maxNumberOfEntriesToReadFromFile) noexcept {
    uint32_t entriesReadFromFile = 0;
    if (m_recFileValid && (maxNumberOfEntriesToReadFromFile > 0)) {
        // Reset any fstream's error states.
        m_recFile.clear();

        while ((m_nextEntryToReadFromRecFile != m_index.end()) && (entriesReadFromFile < maxNumberOfEntriesToReadFromFile)) {
            // Move to corresponding position in the .rec file.
            m_recFile.seekg(static_cast<std::streamoff>(m_nextEntryToReadFromRecFile->second.m_filePosition));

            // Read the corresponding cluon::data::Envelope.
            auto retVal = extractEnvelope(m_recFile);
            if (retVal.first) {
                // Store the envelope in the envelope cache.
                try {
                    std::lock_guard<std::mutex> lck(m_indexMutex);
                    m_nextEntryToReadFromRecFile->second.m_available
                        = m_envelopeCache.emplace(std::make_pair(m_nextEntryToReadFromRecFile->second.m_filePosition, retVal.second)).second;
                } catch (...) {} // LCOV_EXCL_LINE

                m_nextEntryToReadFromRecFile++;
                entriesReadFromFile++;
            }
        }
    }

    return entriesReadFromFile;
}

std::pair<bool, cluon::data::Envelope> Player::getNextEnvelopeToBeReplayed() noexcept {
    bool hasEnvelopeToReturn{false};
    cluon::data::Envelope envelopeToReturn;

    // If at "EOF", either throw exception or autorewind.
    if (m_currentEnvelopeToReplay == m_index.end()) {
        if (!m_autoRewind) {
            return std::make_pair(hasEnvelopeToReturn, envelopeToReturn);
        } else {
            rewind();
        }
    }

    if (m_currentEnvelopeToReplay != m_index.end()) {
        checkAvailabilityOfNextEnvelopeToBeReplayed();

        try {
            {
                std::lock_guard<std::mutex> lck(m_indexMutex);

                cluon::data::Envelope &nextEnvelope = m_envelopeCache[m_currentEnvelopeToReplay->second.m_filePosition];
                envelopeToReturn                    = nextEnvelope;

                m_delay = static_cast<uint32_t>(m_currentEnvelopeToReplay->first - m_previousEnvelopeAlreadyReplayed->first);

                // TODO: Delegate deleting into own thread.
                if (m_previousPreviousEnvelopeAlreadyReplayed != m_index.end()) {
                    auto it = m_envelopeCache.find(m_previousEnvelopeAlreadyReplayed->second.m_filePosition);
                    if (it != m_envelopeCache.end()) {
                        m_envelopeCache.erase(it);
                    }
                }

                m_previousPreviousEnvelopeAlreadyReplayed = m_previousEnvelopeAlreadyReplayed;
                m_previousEnvelopeAlreadyReplayed         = m_currentEnvelopeToReplay++;

                m_numberOfReturnedEnvelopesInTotal++;
            }

            // TODO compensate for internal data processing.

            // If Player is non-threaded, read next entry sequentially.
            if (!m_threading) {
                fillEnvelopeCache(1);
            }

            // Store sample time stamp as int64 to avoid unnecessary copying of Envelopes.
            hasEnvelopeToReturn = true;
        } catch (...) {} // LCOV_EXCL_LINE
    }
    return std::make_pair(hasEnvelopeToReturn, envelopeToReturn);
}

void Player::checkAvailabilityOfNextEnvelopeToBeReplayed() noexcept {
    uint64_t numberOfEntries = 0;
    do {
        {
            try {
                std::lock_guard<std::mutex> lck(m_indexMutex);
                numberOfEntries = m_envelopeCache.size();
            } catch (...) {} // LCOV_EXCL_LINE
        }
        if (0 == numberOfEntries) {
            using namespace std::chrono_literals; // LCOV_EXCL_LINE
            std::this_thread::sleep_for(10ms);    // LCOV_EXCL_LINE
        }
    } while (0 == numberOfEntries);
}

////////////////////////////////////////////////////////////////////////

uint32_t Player::totalNumberOfEnvelopesInRecFile() const noexcept {
    std::lock_guard<std::mutex> lck(m_indexMutex);
    return static_cast<uint32_t>(m_index.size());
}

uint32_t Player::delay() const noexcept {
    std::lock_guard<std::mutex> lck(m_indexMutex);
    // Make sure that delay is not exceeding the specified maximum delay.
    return std::min<uint32_t>(m_delay, Player::MAX_DELAY_IN_MICROSECONDS);
}

void Player::rewind() noexcept {
    if (m_threading) {
        // Stop concurrent thread.
        setEnvelopeCacheFillingRunning(false);
        m_envelopeCacheFillingThread.join();
    }

    computeInitialCacheLevelAndFillCache();

    if (m_threading) {
        // Re-start concurrent thread.
        setEnvelopeCacheFillingRunning(true);
        m_envelopeCacheFillingThread = std::thread(&Player::manageCache, this);
    }
}

void Player::seekTo(float ratio) noexcept {
    if (!(ratio < 0) && !(ratio > 1)) {
        bool enableThreading = m_threading;
        if (m_threading) {
            // Stop concurrent thread.
            setEnvelopeCacheFillingRunning(false);
            m_envelopeCacheFillingThread.join();
        }

        // Read data sequentially.
        m_threading = false;

        resetCaches();
        resetIterators();

        uint32_t numberOfEntriesInIndex = 0;
        try {
            std::lock_guard<std::mutex> lck(m_indexMutex);
            numberOfEntriesInIndex = static_cast<uint32_t>(m_index.size());
        } catch (...) {} // LCOV_EXCL_LINE

        // Fast forward.
        m_numberOfReturnedEnvelopesInTotal = 0;
        std::clog << "[cluon::Player]: Seeking to " << static_cast<float>(numberOfEntriesInIndex) * ratio << "/" << numberOfEntriesInIndex << std::endl;
        if (0 < ratio) {
            for (m_numberOfReturnedEnvelopesInTotal = 0;
                 m_numberOfReturnedEnvelopesInTotal < static_cast<uint32_t>(static_cast<float>(numberOfEntriesInIndex) * ratio) - 1;
                 m_numberOfReturnedEnvelopesInTotal++) {
                m_currentEnvelopeToReplay++;
            }
        }
        try {
            std::lock_guard<std::mutex> lck(m_indexMutex);
            m_nextEntryToReadFromRecFile = m_previousEnvelopeAlreadyReplayed = m_currentEnvelopeToReplay;
        } catch (...) {} // LCOV_EXCL_LINE

        // Refill cache.
        m_envelopeCache.clear();
        fillEnvelopeCache(static_cast<uint32_t>(static_cast<float>(m_desiredInitialLevel) * .3f));

        // Correct iterators if not at the beginning.
        if ((0 < ratio) && (ratio < 1)) {
            getNextEnvelopeToBeReplayed();
        }
        std::clog << "[cluon::Player]: Seeking done." << std::endl;

        if (enableThreading) {
            m_threading = enableThreading;
            // Re-start concurrent thread.
            setEnvelopeCacheFillingRunning(true);
            m_envelopeCacheFillingThread = std::thread(&Player::manageCache, this);
        }
    }
}

bool Player::hasMoreData() const noexcept {
    std::lock_guard<std::mutex> lck(m_indexMutex);
    return hasMoreDataFromRecFile();
}

bool Player::hasMoreDataFromRecFile() const noexcept {
    // File must be successfully opened AND
    //  the Player must be configured as m_autoRewind OR
    //  some entries are left to replay.
    return (m_recFileValid && (m_autoRewind || (m_currentEnvelopeToReplay != m_index.end())));
}

////////////////////////////////////////////////////////////////////////

void Player::setEnvelopeCacheFillingRunning(const bool &running) noexcept {
    std::lock_guard<std::mutex> lck(m_envelopeCacheFillingThreadIsRunningMutex);
    m_envelopeCacheFillingThreadIsRunning = running;
}

bool Player::isEnvelopeCacheFillingRunning() const noexcept {
    std::lock_guard<std::mutex> lck(m_envelopeCacheFillingThreadIsRunningMutex);
    return m_envelopeCacheFillingThreadIsRunning;
}

void Player::manageCache() noexcept {
    uint8_t statisticsCounter = 0;
    float refillMultiplicator = 1.1f;
    uint32_t numberOfEntries  = 0;

    while (isEnvelopeCacheFillingRunning()) {
        try {
            std::lock_guard<std::mutex> lck(m_indexMutex);
            numberOfEntries = static_cast<uint32_t>(m_envelopeCache.size());
        } catch (...) {} // LCOV_EXCL_LINE

        // Check if refilling of the cache is needed.
        refillMultiplicator = checkRefillingCache(numberOfEntries, refillMultiplicator);
        (void)refillMultiplicator;

        // Manage cache at 10 Hz.
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);

        // Publish some statistics at 1 Hz.
        if (0 == ((++statisticsCounter) % 10)) {
            uint64_t numberOfReturnedEnvelopesInTotal = 0;
            uint32_t totalNumberOfEnvelopes           = 0;
            try {
                // m_numberOfReturnedEnvelopesInTotal is modified in a different thread.
                std::lock_guard<std::mutex> lck(m_indexMutex);
                numberOfReturnedEnvelopesInTotal = m_numberOfReturnedEnvelopesInTotal;
                totalNumberOfEnvelopes           = static_cast<uint32_t>(m_index.size());
            } catch (...) {} // LCOV_EXCL_LINE

            try {
                std::lock_guard<std::mutex> lck(m_playerListenerMutex);
                if (nullptr != m_playerListener) {
                    cluon::data::PlayerStatus ps;
                    ps.state(2); // State: "playback"
                    ps.numberOfEntries(totalNumberOfEnvelopes);
                    ps.currentEntryForPlayback(static_cast<uint32_t>(numberOfReturnedEnvelopesInTotal));
                    m_playerListener(ps);
                }
            } catch (...) {} // LCOV_EXCL_LINE

            statisticsCounter = 0;
        }
    }
}

float Player::checkRefillingCache(const uint32_t &numberOfEntries, float refillMultiplicator) noexcept {
    // If filling level is around 35%, pour in more from the recording.
    if (numberOfEntries < 0.35 * m_desiredInitialLevel) {
        const uint32_t entriesReadFromFile = fillEnvelopeCache(static_cast<uint32_t>(refillMultiplicator * static_cast<float>(m_desiredInitialLevel)));
        if (entriesReadFromFile > 0) {
            std::clog << "[cluon::Player]: Number of entries in cache: " << numberOfEntries << ". " << entriesReadFromFile << " added to cache. "
                      << m_envelopeCache.size() << " entries available." << std::endl;
            refillMultiplicator *= 1.25f;
        }
    }
    return refillMultiplicator;
}

} // namespace cluon
