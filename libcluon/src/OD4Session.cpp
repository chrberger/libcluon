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

#include "cluon/OD4Session.hpp"
#include "cluon/Envelope.hpp"
#include "cluon/FromProtoVisitor.hpp"
#include "cluon/TerminateHandler.hpp"
#include "cluon/Time.hpp"

#include <iostream>
#include <sstream>
#include <thread>

namespace cluon {

OD4Session::OD4Session(uint16_t CID, std::function<void(cluon::data::Envelope &&envelope)> delegate) noexcept
    : m_receiver{nullptr}
    , m_sender{"225.0.0." + std::to_string(CID), 12175}
    , m_delegate(std::move(delegate))
    , m_mapOfDataTriggeredDelegatesMutex{}
    , m_mapOfDataTriggeredDelegates{} {
    m_receiver = std::make_unique<cluon::UDPReceiver>(
        "225.0.0." + std::to_string(CID), 12175, [this](std::string &&data, std::string &&from, std::chrono::system_clock::time_point &&timepoint) {
            this->callback(std::move(data), std::move(from), std::move(timepoint));
    });
}

void OD4Session::timeTrigger(float freq, std::function<bool()> delegate) noexcept {
    if (nullptr != delegate) {
        bool delegateIsRunning{true};
        const int64_t TIME_SLICE_IN_MILLISECONDS{static_cast<uint32_t>(1000 / ((freq > 0) ? freq : 1.0f))};
        do {
            cluon::data::TimeStamp before{cluon::time::now()};
            try {
                delegateIsRunning = delegate();
            } catch (...) {
                delegateIsRunning = false; // delegate threw exception.
            }
            cluon::data::TimeStamp after{cluon::time::now()};

            const int64_t beforeInMicroseconds{cluon::time::toMicroseconds(before)};
            const int64_t afterInMicroseconds{cluon::time::toMicroseconds(after)};

            const int64_t timeSpent{(afterInMicroseconds > beforeInMicroseconds) ? (afterInMicroseconds - beforeInMicroseconds) / 1000 : 0};
            const int64_t timeToSleepInMilliseconds{TIME_SLICE_IN_MILLISECONDS - timeSpent};

            // Sleep the remaining time.
            if ((timeToSleepInMilliseconds > 0) && (timeToSleepInMilliseconds <= TIME_SLICE_IN_MILLISECONDS)) {
                std::this_thread::sleep_for(std::chrono::duration<int64_t, std::milli>(timeToSleepInMilliseconds));
            } else {
                std::cerr << "[cluon::OD4Session]: time-triggered delegate violated allocated time slice." << std::endl;
            }
        } while (delegateIsRunning && !TerminateHandler::instance().isTerminated.load());
    }
}

bool OD4Session::dataTrigger(int32_t messageIdentifier, std::function<void(cluon::data::Envelope &&envelope)> delegate) noexcept {
    bool retVal{false};
    if (nullptr == m_delegate) {
        try {
            std::lock_guard<std::mutex> lck{m_mapOfDataTriggeredDelegatesMutex};
            if ((nullptr == delegate) && (m_mapOfDataTriggeredDelegates.count(messageIdentifier) > 0)) {
                auto element = m_mapOfDataTriggeredDelegates.find(messageIdentifier);
                if (element != m_mapOfDataTriggeredDelegates.end()) {
                    m_mapOfDataTriggeredDelegates.erase(element);
                }
            } else {
                m_mapOfDataTriggeredDelegates[messageIdentifier] = delegate;
            }
            retVal = true;
        } catch (...) {} // LCOV_EXCL_LINE
    }
    return retVal;
}

void OD4Session::callback(std::string &&data, std::string && /*from*/, std::chrono::system_clock::time_point &&timepoint) noexcept {
    std::stringstream sstr(data);
    auto retVal = extractEnvelope(sstr);

    if (retVal.first) {
        cluon::data::Envelope env{retVal.second};
        env.received(cluon::time::convert(timepoint));

        // "Catch all"-delegate.
        if (nullptr != m_delegate) {
            m_delegate(std::move(env));
        } else {
            try {
                // Data triggered-delegates.
                std::lock_guard<std::mutex> lck{m_mapOfDataTriggeredDelegatesMutex};
                if (m_mapOfDataTriggeredDelegates.count(env.dataType()) > 0) {
                    m_mapOfDataTriggeredDelegates[env.dataType()](std::move(env));
                }
            } catch (...) {} // LCOV_EXCL_LINE
        }
    }
}

void OD4Session::send(cluon::data::Envelope &&envelope) noexcept {
    sendInternal(cluon::serializeEnvelope(std::move(envelope)));
}

void OD4Session::sendInternal(std::string &&dataToSend) noexcept {
    m_sender.send(std::move(dataToSend));
}

bool OD4Session::isRunning() noexcept {
    return m_receiver->isRunning();
}

} // namespace cluon
