/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_ENVELOPE_HPP
#define CLUON_ENVELOPE_HPP

#include "cluon/FromProtoVisitor.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/cluonDataStructures.hpp"

#include <cstring>
#include <istream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace cluon {

/**
 * This method transforms a given Envelope to a string representation to be
 * sent to an OpenDaVINCI session.
 *
 * @param envelope Envelope with payload to be sent.
 * @return String representation of the Envelope to be sent to OpenDaVINCI v4.
 */
inline std::string serializeEnvelope(cluon::data::Envelope &&envelope) noexcept {
    std::string dataToSend;
    {
        std::stringstream sstr;

        cluon::ToProtoVisitor protoEncoder;
        envelope.accept(protoEncoder);

        const std::string tmp{protoEncoder.encodedData()};
        uint32_t length{static_cast<uint32_t>(tmp.size())};
        length <<= 8;
        length = htole32(length);

        // Add OD4 header.
        constexpr unsigned char OD4_HEADER_BYTE0 = 0x0D;
        constexpr unsigned char OD4_HEADER_BYTE1 = 0xA4;
        sstr.put(static_cast<char>(OD4_HEADER_BYTE0));
        auto posByte1 = sstr.tellp();
        sstr.write(reinterpret_cast<char *>(&length), static_cast<std::streamsize>(sizeof(uint32_t)));
        auto posByte5 = sstr.tellp();
        sstr.seekp(posByte1);
        sstr.put(static_cast<char>(OD4_HEADER_BYTE1));
        sstr.seekp(posByte5);

        // Write payload.
        sstr.write(tmp.data(), static_cast<std::streamsize>(tmp.size()));

        dataToSend = sstr.str();
    }
    return dataToSend;
}

/**
 * This method extracts an Envelope from the given istream that holds bytes in
 * format:
 *
 *    0x0D 0xA4 LEN0 LEN1 LEN2 Proto-encoded cluon::data::Envelope
 *
 * 0xA4 LEN0 LEN1 LEN2 are little Endian.
 *
 * @param in Stream to read from.
 * @return cluon::data::Envelope.
 */
inline std::pair<bool, cluon::data::Envelope> extractEnvelope(std::istream &in) noexcept {
    bool retVal{false};
    cluon::data::Envelope env;
    if (in.good()) {
        constexpr uint8_t OD4_HEADER_SIZE{5};
        std::vector<char> buffer;
        buffer.reserve(OD4_HEADER_SIZE);
#ifdef WIN32                                           // LCOV_EXCL_LINE
        buffer.clear();                                // LCOV_EXCL_LINE
        retVal = true;                                 // LCOV_EXCL_LINE
        for (uint8_t i{0}; i < OD4_HEADER_SIZE; i++) { // LCOV_EXCL_LINE
            char c;                                    // LCOV_EXCL_LINE
            in.get(c);                                 // LCOV_EXCL_LINE
            retVal &= in.good();                       // LCOV_EXCL_LINE
            buffer.push_back(c);                       // LCOV_EXCL_LINE
        }
        if (retVal) { // LCOV_EXCL_LINE
#else                 // LCOV_EXCL_LINE
        in.read(&buffer[0], OD4_HEADER_SIZE);
        if (OD4_HEADER_SIZE == in.gcount()) {
#endif
            if ((0x0D == static_cast<uint8_t>(buffer[0])) && (0xA4 == static_cast<uint8_t>(buffer[1]))) {
                const uint32_t LENGTH{le32toh(*reinterpret_cast<uint32_t *>(&buffer[1])) >> 8};
                buffer.reserve(LENGTH);
#ifdef WIN32                                           // LCOV_EXCL_LINE
                buffer.clear();                        // LCOV_EXCL_LINE
                for (uint32_t i{0}; i < LENGTH; i++) { // LCOV_EXCL_LINE
                    char c;                            // LCOV_EXCL_LINE
                    in.get(c);                         // LCOV_EXCL_LINE
                    retVal &= in.good();               // LCOV_EXCL_LINE
                    buffer.push_back(c);               // LCOV_EXCL_LINE
                }
#else // LCOV_EXCL_LINE
                in.read(&buffer[0], static_cast<std::streamsize>(LENGTH));
                retVal = static_cast<int32_t>(LENGTH) == in.gcount();
#endif
                if (retVal) {
                    std::stringstream sstr(std::string(buffer.begin(), buffer.begin() + static_cast<std::streamsize>(LENGTH)));
                    cluon::FromProtoVisitor protoDecoder;
                    protoDecoder.decodeFrom(sstr, env);
                }
            }
        }
    }
    return std::make_pair(retVal, env);
}

/**
 * @return Extract a given Envelope's payload into the desired type.
 */
template <typename T>
inline T extractMessage(cluon::data::Envelope &&envelope) noexcept {
    cluon::FromProtoVisitor decoder;

    std::stringstream sstr(envelope.serializedData());
    decoder.decodeFrom(sstr);

    T msg;
    msg.accept(decoder);

    return msg;
}

} // namespace cluon

#endif
