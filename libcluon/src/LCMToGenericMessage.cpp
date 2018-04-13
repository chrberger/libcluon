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

#include "cluon/LCMToGenericMessage.hpp"
#include "cluon/FromLCMVisitor.hpp"
#include "cluon/MessageParser.hpp"

#include <array>
#include <iostream>
#include <sstream>

// clang-format off
#ifdef WIN32
    #undef be32toh
    #define be32toh(x) __ntohl(x)

    #include <cstdint>
    #include <cstring>
    uint32_t __ntohl(const uint32_t v) {
        uint8_t d[4] = {};
        std::memmove(&d, &v, sizeof(d));

        return ((uint32_t) d[3] << 0)
             | ((uint32_t) d[2] << 8)
             | ((uint32_t) d[1] << 16)
             | ((uint32_t) d[0] << 24);
    }
#endif
// clang-format on

namespace cluon {

int32_t LCMToGenericMessage::setMessageSpecification(const std::string &ms) noexcept {
    int32_t retVal{-1};

    m_listOfMetaMessages.clear();
    m_scopeOfMetaMessages.clear();

    cluon::MessageParser mp;
    auto parsingResult = mp.parse(ms);
    if (cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == parsingResult.second) {
        m_listOfMetaMessages = parsingResult.first;
        for (const auto &mm : m_listOfMetaMessages) { m_scopeOfMetaMessages[mm.messageName()] = mm; }
        retVal = static_cast<int32_t>(m_listOfMetaMessages.size());
    }
    return retVal;
}

cluon::GenericMessage LCMToGenericMessage::getGenericMessage(const std::string &data) noexcept {
    cluon::GenericMessage gm;

    if (!m_listOfMetaMessages.empty()) {
        constexpr uint8_t LCM_HEADER_SIZE{4 /*magic number*/ + 4 /*sequence number*/ + 1 /*'\0' after channel name*/};
        if (LCM_HEADER_SIZE < data.size()) {
            // First, read magic number.
            constexpr uint32_t MAGIC_NUMBER_LCM2{0x4c433032};
            uint32_t offset{0};
            uint32_t magicNumber{0};
            {
                std::stringstream sstr{std::string(&data[offset], 4)};
                // clang-format off
                sstr.read(reinterpret_cast<char *>(&magicNumber), sizeof(uint32_t)); /* Flawfinder: ignore */ // NOLINT
                // clang-format on
                magicNumber = be32toh(magicNumber);
            }
            if (MAGIC_NUMBER_LCM2 == magicNumber) {
                offset += 4;

                // Next, read sequence number in case of fragmented data.
                uint32_t sequenceNumber{0};
                {
                    std::stringstream sstr{std::string(&data[offset], 4)};
                    // clang-format off
                    sstr.read(reinterpret_cast<char *>(&sequenceNumber), sizeof(uint32_t)); /* Flawfinder: ignore */ // NOLINT
                    // clang-format on
                    sequenceNumber = be32toh(sequenceNumber);
                }
                // Only support for non-fragmented messages.
                if (0 == sequenceNumber) {
                    offset += 4;

                    const std::string::size_type START_POSITION = offset;
                    std::string::size_type pos                  = data.find('\0', START_POSITION); // Extract channel name.
                    if (std::string::npos != pos) {
                        const std::string CHANNEL_NAME(data.substr(START_POSITION, (pos - START_POSITION)));

                        // Next, find the MetaMessage corresponding to the channel name
                        // and create a Message therefrom based on the decoded LCM data.
                        if ((0 < m_scopeOfMetaMessages.count(CHANNEL_NAME)) && (std::string::npos != (pos + 1))) {
                            // data[offset+i] marks now the beginning of the payload to be decoded.
                            std::stringstream sstr{data.substr(pos + 1)};

                            cluon::FromLCMVisitor fromLCM;
                            fromLCM.decodeFrom(sstr);

                            gm.createFrom(m_scopeOfMetaMessages[CHANNEL_NAME], m_listOfMetaMessages);
                            gm.accept(fromLCM);
                        }
                    }
                }
            }
        }
    }

    return gm;
}

} // namespace cluon
