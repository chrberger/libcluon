/*
 * Copyright (C) 2018  Christian Berger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "cluon/cluonDataStructures.hpp"

#include <cstdint>
#include <deque>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

namespace cluon {

class IndexEntry {
    public:
        IndexEntry() noexcept;
        IndexEntry(const int64_t &sampleTimeStamp, const uint64_t &filePosition) noexcept;

    public:
        int64_t m_sampleTimeStamp;
        uint64_t m_filePosition;
        bool m_available;
};

class LIBCLUON_API Player {
    private:
        enum {
            ONE_MILLISECOND_IN_MICROSECONDS = 1000,
            ONE_SECOND_IN_MICROSECONDS = 1000 * ONE_MILLISECOND_IN_MICROSECONDS,
            MAX_DELAY_IN_MICROSECONDS = 1 * ONE_SECOND_IN_MICROSECONDS,
            LOOK_AHEAD_IN_S = 30,
            MIN_ENTRIES_FOR_LOOK_AHEAD = 5000,
        };

   private:
    Player(const Player &) = delete;
    Player(Player &&)      = delete;
    Player &operator=(Player &&) = delete;
    Player &operator=(const Player &other) = delete;

    public:
        /**
         * Constructor.
         *
         * @param file File to play.
         * @param autoRewind True if the file should be rewind at EOF.
         * @param threading If set to true, player will load new envelopes from the files in background.
         */
        Player(const std::string &file, const bool &autoRewind, const bool &threading) noexcept;
        ~Player();

        /**
         * @return Pair of bool and next cluon::data::Envelope to be replayed;
         *         if bool is false, no next Envelope is available.
         */
        std::pair<bool, cluon::data::Envelope> getNextEnvelopeToBeReplayed() noexcept;

        /**
         * @return real delay in microseconds to be waited before the next cluon::data::Envelope should be delivered.
         */
        uint32_t getDelay() const noexcept;

        /**
         * @return delay in microseconds to be waited before the next cluon::data::Envelope should be delivered correct by the internal processing time.
         */
        uint32_t getCorrectedDelay() const noexcept;

        /**
         * @return true if there is more data to replay.
         */
        bool hasMoreData() const noexcept;

        /**
         * This method rewinds the iterators.
         */
        void rewind() noexcept;

        void seekTo(float ratio) noexcept;

        /**
         * @return total amount of cluon::data::Envelopes in the .rec file.
         */
        uint32_t getTotalNumberOfEnvelopesInRecFile() const noexcept;

    private:
        // Internal methods without Lock.
        bool hasMoreDataFromRecFile() const noexcept;

        /**
         * This method initializes the global index where the sample
         * time stamps are sorted chronocally and mapped to the 
         * corresponding cluon::data::Envelope in the rec file.
         */
        void initializeIndex() noexcept;

        /**
         * This method computes the initially required amount of
         * cluon::data::Envelope in the cache and fill the cache accordingly.
         */
        void computeInitialCacheLevelAndFillCache() noexcept;

        /**
         * This method clears all caches.
         */
        void resetCaches() noexcept;

        /**
         * This method resets the iterators.
         */
        inline void resetIterators() noexcept;

        /**
         * This method fills the cache by trying to read up
         * to maxNumberOfEntriesToReadFromFile from the rec file.
         *
         * @param maxNumberOfEntriesToReadFromFile Maximum number of entries to be read from file.
         * @return Number of entries read from file.
         */
        uint32_t fillEnvelopeCache(const uint32_t &maxNumberOfEntriesToReadFromFile) noexcept;

        /**
         * This method checks the availability of the next cluon::data::Envelope
         * to be replayed from the cache.
         */
        inline void checkAvailabilityOfNextEnvelopeToBeReplayed() noexcept;

    private: // Data for the Player.
        bool m_threading;

        std::string m_file;

        // Handle to .rec file.
        std::fstream m_recFile;
        bool m_recFileValid;

    private: // Player states.
        bool m_autoRewind;

    private: // Index and cache management.
        // Global index: Mapping SampleTimeStamp --> cache entry (holding the actual content from .rec file).
        mutable std::mutex m_indexMutex;
        std::multimap<int64_t, IndexEntry> m_index;

        // Pointers to the current envelope to be replayed and the
        // envelope that has be replayed from the global index.
        std::multimap<int64_t, IndexEntry>::iterator m_previousPreviousEnvelopeAlreadyReplayed;
        std::multimap<int64_t, IndexEntry>::iterator m_previousEnvelopeAlreadyReplayed;
        std::multimap<int64_t, IndexEntry>::iterator m_currentEnvelopeToReplay;

        // Information about the index.
        std::multimap<int64_t, IndexEntry>::iterator m_nextEntryToReadFromRecFile;

        uint32_t m_desiredInitialLevel;

        // Fields to compute replay throughput for cache management.
        cluon::data::TimeStamp m_firstTimePointReturningAEnvelope;
        uint64_t m_numberOfReturnedEnvelopesInTotal;

        uint32_t m_delay;
        uint32_t m_correctedDelay;

    private:
        /**
         * This method sets the state of the envelopeCacheFilling thread.
         *
         * @param running False if the thread to fill the Envelope cache shall be joined.
         */
        void setEnvelopeCacheFillingRunning(const bool &running) noexcept;
        bool isEnvelopeCacheFillingRunning() const noexcept;

        /**
         * This method manages the cache.
         */
        void manageCache() noexcept;

        /**
         * This method checks whether the cache needs to be refilled.
         *
         * @param numberOfEntries Number of entries in cache.
         * @param refillMultiplicator Multiplicator to modify the amount of envelopes to be refilled.
         * @return Modified refillMultiplicator recommedned to be used next time 
         */
        float checkRefillingCache(const uint32_t &numberOfEntries, float refillMultiplicator) noexcept;

    private:
        mutable std::mutex m_envelopeCacheFillingThreadIsRunningMutex;
        bool m_envelopeCacheFillingThreadIsRunning;
        std::thread m_envelopeCacheFillingThread;

        // Mapping of pos_type (within .rec file) --> cluon::data::Envelope (read from .rec file).
        std::map<uint64_t, cluon::data::Envelope> m_envelopeCache;

    public:
        void setPlayerListener(std::function<void(cluon::data::PlayerStatus playerStatus)> playerListener) noexcept;

    private:
        std::mutex m_playerListenerMutex;
        std::function<void(cluon::data::PlayerStatus playerStatus)> m_playerListener{nullptr};
};

}

#endif
