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

#ifndef OD4SESSION_HPP
#define OD4SESSION_HPP

#include "cluon/Time.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/UDPReceiver.hpp"
#include "cluon/UDPSender.hpp"
#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

namespace cluon {
/**
This class provides an interface to an OpenDaVINCI v4 session. An OpenDaVINCI
v4 session allows the automatic exchange of time-stamped Envelopes carrying
user-defined messages usually using UDP multicast.

There are two ways to participate in an OpenDaVINCI session. Variant A is simply
calling a user-supplied lambda whenever a new Envelope is received:

\code{.cpp}
cluon::OD4Session od4{111, [](cluon::data::Envelope &&envelope){ std::cout << "Received cluon::Envelope" << std::endl;}
};

// Do something in parallel.

MyMessage msg;
od4.send(msg);
\endcode

Variant B allows a more fine-grained setup where you specify the Envelopes of interest:

\code{.cpp}
cluon::OD4Session od4{111};

od4.dataTrigger(cluon::data::TimeStamp::ID(), [](cluon::data::Envelope &&envelope){ std::cout << "Received cluon::data::TimeStamp" << std::endl;});
od4.dataTrigger(MyMessage::ID(), [](cluon::data::Envelope &&envelope){ std::cout << "Received MyMessage" << std::endl;});

// Do something in parallel.

MyMessage msg;
od4.send(msg);
\endcode

Next to receive Envelopes, OD4Session can call a user-supplied lambda in a time-triggered
way. The lambda is executed as long as it does not return false or throws an exception
that is then caught in the method timeTrigger and the method is exited:

\code{.cpp}
cluon::OD4Session od4{111};

const float FREQ{10}; // 10 Hz.
od4.timeTrigger(FREQ, [](){
  // Do something time-triggered.
  return false;
}); // This call blocks until the lambda returns false.
\endcode
*/
class LIBCLUON_API OD4Session {
   private:
    OD4Session(const OD4Session &) = delete;
    OD4Session(OD4Session &&)      = delete;
    OD4Session &operator=(const OD4Session &) = delete;
    OD4Session &operator=(OD4Session &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param CID OpenDaVINCI v4 session identifier [1 .. 254]
     * @param delegate Function to call on newly arriving Envelopes ("catch-all");
     *        if a nullptr is passed, the method dataTrigger can be used to set
     *        message specific delegates. Please note that it is NOT possible
     *        to have both: a delegate for "catch-all" and the data-triggered ones.
     */
    OD4Session(uint16_t CID, std::function<void(cluon::data::Envelope &&envelope)> delegate = nullptr) noexcept;

    /**
     * This method will send a given Envelope to this OpenDaVINCI v4 session.
     *
     * @param envelope to be sent.
     */
    void send(cluon::data::Envelope &&envelope) noexcept;

    /**
     * This method sets a delegate to be called data-triggered on arrival
     * of a new Envelope for a given message identifier.
     *
     * @param messageIdentifier Message identifier to assign a delegate.
     * @param delegate Function to call on newly arriving Envelopes; setting it to nullptr will erase it.
     * @return true if the given delegate could be successfully set or unset.
     */
    bool dataTrigger(int32_t messageIdentifier, std::function<void(cluon::data::Envelope &&envelope)> delegate) noexcept;

    /**
     * This method sets a delegate to be called time-triggered using the
     * specified frequency until the delegate returns false. This method
     * blocks until the delegate has returned false or threw an exception.
     * Thus, this method is typically called as last statement in a main
     * function of a program.
     *
     * @param freq Frequency in Hertz to run the given delegate.
     * @param delegate Function to call according to the given frequency.
     */
    void timeTrigger(float freq, std::function<bool()> delegate) noexcept;

    /**
     * This method will send a given message to this OpenDaVINCI v4 session.
     *
     * @param message Message to be sent.
     * @param sampleTimeStamp Time point when this sample to be sent was captured (default = sent time point).
     * @param senderStamp Optional sender stamp (default = 0).
     */
    template <typename T>
    void send(T &message, const cluon::data::TimeStamp &sampleTimeStamp = cluon::data::TimeStamp(), uint32_t senderStamp = 0) noexcept {
        try {
            std::lock_guard<std::mutex> lck(m_senderMutex);
            cluon::ToProtoVisitor protoEncoder;

            cluon::data::Envelope envelope;
            {
                envelope.dataType(static_cast<int32_t>(message.ID()));
                message.accept(protoEncoder);
                envelope.serializedData(protoEncoder.encodedData());
                envelope.sent(cluon::time::now());
                envelope.sampleTimeStamp((0 == (sampleTimeStamp.seconds() + sampleTimeStamp.microseconds())) ? envelope.sent() : sampleTimeStamp);
                envelope.senderStamp(senderStamp);
            }

            send(std::move(envelope));
        } catch (...) {} // LCOV_EXCL_LINE
    }

   public:
    bool isRunning() noexcept;

   private:
    void callback(std::string &&data, std::string &&from, std::chrono::system_clock::time_point &&timepoint) noexcept;
    void sendInternal(std::string &&dataToSend) noexcept;

   private:
    std::unique_ptr<cluon::UDPReceiver> m_receiver;
    cluon::UDPSender m_sender;

    std::mutex m_senderMutex{};

    std::function<void(cluon::data::Envelope &&envelope)> m_delegate{nullptr};

    std::mutex m_mapOfDataTriggeredDelegatesMutex{};
    std::map<int32_t, std::function<void(cluon::data::Envelope &&envelope)>> m_mapOfDataTriggeredDelegates{};
};

} // namespace cluon
#endif
