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
#include "cluon/FromProtoVisitor.hpp"

#include <array>
#include <iostream>
#include <sstream>

namespace cluon {

OD4Session::OD4Session(uint16_t CID, std::function<void(cluon::data::Envelope &&envelope)> delegate) noexcept
    : m_receiver{"225.0.0." + std::to_string(CID),
                 12175,
                 [this](std::string &&data, std::string &&from, std::chrono::system_clock::time_point &&timepoint) {
                     this->callback(std::move(data), std::move(from), std::move(timepoint));
                 }}
    , m_sender{"225.0.0." + std::to_string(CID), 12175}
    , m_delegate(delegate) {}

void OD4Session::callback(std::string &&data,
                          std::string &&from,
                          std::chrono::system_clock::time_point &&timepoint) noexcept {
    cluon::data::TimeStamp receivedAt;
    {
        // Transform chrono time representation to same behavior as gettimeofday.
        typedef std::chrono::duration<int32_t> seconds_type;
        typedef std::chrono::duration<int64_t, std::micro> microseconds_type;

        auto duration                = timepoint.time_since_epoch();
        seconds_type s               = std::chrono::duration_cast<seconds_type>(duration);
        microseconds_type us         = std::chrono::duration_cast<microseconds_type>(duration);
        microseconds_type partial_us = us - std::chrono::duration_cast<microseconds_type>(s);

        receivedAt.seconds(static_cast<int32_t>(s.count())).microseconds(static_cast<int32_t>(partial_us.count()));
    }

    if (nullptr != m_delegate) {
        constexpr uint8_t OD4_HEADER_SIZE{5};
        const std::string protoEncodedEnvelope{data};
        if (OD4_HEADER_SIZE <= protoEncodedEnvelope.size()) {
            char byte0{protoEncodedEnvelope.at(0)};
            char byte1{protoEncodedEnvelope.at(1)};
            uint32_t length{0};
            {
                std::stringstream sstr{std::string(&protoEncodedEnvelope[1], 4)};
                sstr.read(reinterpret_cast<char *>(&length), sizeof(uint32_t)); /* Flawfinder: ignore */ // NOLINT
                length = le32toh(length);
                length >>= 8;
            }
            std::string input;
            if ((0x0D == static_cast<uint8_t>(byte0)) && (0xA4 == static_cast<uint8_t>(byte1))
                && (length == protoEncodedEnvelope.size() - OD4_HEADER_SIZE)) {
                cluon::data::Envelope env;
                {
                    std::stringstream sstr{protoEncodedEnvelope.substr(OD4_HEADER_SIZE)};
                    cluon::FromProtoVisitor protoDecoder;
                    protoDecoder.decodeFrom(sstr);
                    env.accept(protoDecoder);
                }
                env.received(receivedAt);
                m_delegate(std::move(env));
            }
        }
    } else {
        std::cout << "[cluon::OD4Session] Received " << data.size() << " bytes from " << from << " at "
                  << receivedAt.seconds() << "." << receivedAt.microseconds() << "." << std::endl;
    }
}

void OD4Session::send(cluon::data::Envelope &&envelope) noexcept {
    sendInternal(cluon::OD4Session::serializeAsOD4Container(std::move(envelope)));
}

std::string OD4Session::serializeAsOD4Container(cluon::data::Envelope &&envelope) noexcept {
    std::string dataToSend;
    {
        cluon::ToProtoVisitor protoEncoder;
        envelope.accept(protoEncoder);

        const std::string tmp{protoEncoder.encodedData()};
        uint32_t length{static_cast<uint32_t>(tmp.size())};
        length = htole32(length);

        // Add OpenDaVINCI header.
        std::array<char, 5> header;
        header[0] = static_cast<char>(0x0D);
        header[1] = static_cast<char>(0xA4);
        header[2] = *(reinterpret_cast<char *>(&length) + 0);
        header[3] = *(reinterpret_cast<char *>(&length) + 1);
        header[4] = *(reinterpret_cast<char *>(&length) + 2);

        std::stringstream sstr;
        sstr.write(header.data(), static_cast<std::streamsize>(header.size()));
        sstr.write(tmp.data(), static_cast<std::streamsize>(tmp.size()));
        dataToSend = sstr.str();
    }
    return dataToSend;
}

void OD4Session::sendInternal(std::string &&dataToSend) noexcept {
    m_sender.send(std::move(dataToSend));
}

bool OD4Session::isRunning() noexcept {
    return m_receiver.isRunning();
}

} // namespace cluon
