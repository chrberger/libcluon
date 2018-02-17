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

#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include "cluon/FromProtoVisitor.hpp"
#include "cluon/cluonDataStructures.hpp"

#include <cstring>
#include <istream>
#include <sstream>
#include <vector>

#include <iostream>

namespace cluon {

/**
 * This method extracts an Envelope from the given istream.
 *
 * @param in Stream to read from.
 * @return cluon::data::Envelope.
 */
inline cluon::data::Envelope extractEnvelope(std::istream &in) noexcept {
    cluon::data::Envelope env;
    if (in.good()) {
        constexpr uint8_t OD4_HEADER_SIZE{5};
        std::vector<char> buffer;
        buffer.reserve(OD4_HEADER_SIZE);
        if (OD4_HEADER_SIZE == in.readsome(&buffer[0], OD4_HEADER_SIZE)) {
            const uint32_t LENGTH{le32toh(*reinterpret_cast<uint32_t*>(&buffer[1])) >> 8};
            if (   (0x0D == static_cast<uint8_t>(buffer[0])) 
                && (0xA4 == static_cast<uint8_t>(buffer[1]))) {
                buffer.reserve(LENGTH);
                if (LENGTH == in.readsome(&buffer[0], LENGTH)) {
                    std::stringstream sstr;
                    sstr.rdbuf()->pubsetbuf(&buffer[0], LENGTH); // Avoid duplicating the read data.
                    cluon::FromProtoVisitor protoDecoder;
                    protoDecoder.decodeFrom(sstr);
                    env.accept(protoDecoder);
                }
            }
        }
    }
    return env;
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
