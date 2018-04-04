/*
 * Copyright (C) 2017-2018  Christian Berger
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

#include "cluon/EnvelopeConverter.hpp"
#include "cluon/Envelope.hpp"
#include "cluon/FromJSONVisitor.hpp"
#include "cluon/FromProtoVisitor.hpp"
#include "cluon/GenericMessage.hpp"
#include "cluon/MessageParser.hpp"
#include "cluon/ToJSONVisitor.hpp"
#include "cluon/ToProtoVisitor.hpp"

#include <algorithm>
#include <sstream>
#include <utility>

namespace cluon {

int32_t EnvelopeConverter::setMessageSpecification(const std::string &ms) noexcept {
    int32_t retVal{-1};

    m_listOfMetaMessages.clear();
    m_scopeOfMetaMessages.clear();

    cluon::MessageParser mp;
    auto parsingResult = mp.parse(ms);
    if (cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == parsingResult.second) {
        m_listOfMetaMessages = parsingResult.first;
        for (const auto &mm : m_listOfMetaMessages) { m_scopeOfMetaMessages[mm.messageIdentifier()] = mm; }
        retVal = static_cast<int32_t>(m_listOfMetaMessages.size());
    }
    return retVal;
}

std::string EnvelopeConverter::getJSONFromProtoEncodedEnvelope(const std::string &protoEncodedEnvelope) noexcept {
    std::string retVal{"{}"};
    if (!m_listOfMetaMessages.empty()) {
        cluon::data::Envelope envelope;
        std::stringstream sstr(protoEncodedEnvelope);
        constexpr uint8_t OD4_HEADER_SIZE{5};
        if (OD4_HEADER_SIZE < protoEncodedEnvelope.size()) {
            // Try decoding complete OD4-encoded Envelope including header.
            constexpr uint8_t byte0{0x0D};
            constexpr uint8_t byte1{0xA4};
            if ((static_cast<uint8_t>(protoEncodedEnvelope.at(0)) == byte0) && (static_cast<uint8_t>(protoEncodedEnvelope.at(1)) == byte1)) {
                uint32_t length = (*reinterpret_cast<const uint32_t *>(protoEncodedEnvelope.data() + 1));
                length          = le32toh(length) >> 8;
                if ((OD4_HEADER_SIZE + length) == protoEncodedEnvelope.size()) {
                    auto result{extractEnvelope(sstr)};
                    if (result.first) {
                        envelope = result.second;
                    }
                }
            }
        }

        if (0 == envelope.dataType()) {
            // Directly decoding complete OD4 container failed, try decoding
            // without header.
            cluon::FromProtoVisitor protoDecoder;
            protoDecoder.decodeFrom(sstr);
            envelope.accept(protoDecoder);
        }

        retVal = getJSONFromEnvelope(envelope);
    }
    return retVal;
}

std::string EnvelopeConverter::getJSONFromEnvelope(cluon::data::Envelope &envelope) noexcept {
    std::string retVal{"{}"};
    if (!m_listOfMetaMessages.empty()) {
        if (0 < m_scopeOfMetaMessages.count(envelope.dataType())) {
            // First, create JSON from Envelope.
            constexpr bool OUTER_CURLY_BRACES{false};
            // Ignore field 2 (= serializedData) as it will be replaced below.
            const std::map<uint32_t, bool> mask{{2, false}};
            ToJSONVisitor envelopeToJSON{OUTER_CURLY_BRACES, mask};
            envelope.accept(envelopeToJSON);

            std::stringstream sstr{envelope.serializedData()};
            cluon::FromProtoVisitor protoDecoder;
            protoDecoder.decodeFrom(sstr);

            // Now, create JSON from payload.
            cluon::MetaMessage payload{m_scopeOfMetaMessages[envelope.dataType()]};
            cluon::GenericMessage gm;

            // Create "empty" GenericMessage from this MetaMessage.
            gm.createFrom(payload, m_listOfMetaMessages);

            // Set values in the newly created GenericMessage from ProtoDecoder.
            gm.accept(protoDecoder);

            ToJSONVisitor payloadToJSON{OUTER_CURLY_BRACES};
            gm.accept(payloadToJSON);

            std::string tmp{payload.messageName()};
            std::replace(tmp.begin(), tmp.end(), '.', '_');

            retVal = '{' + envelopeToJSON.json() + ',' + '\n' + '"' + tmp + '"' + ':' + '{' + payloadToJSON.json() + '}' + '}';
        }
    }
    return retVal;
}

std::string EnvelopeConverter::getProtoEncodedEnvelopeFromJSONWithoutTimeStamps(const std::string &json, int32_t messageIdentifier) noexcept {
    std::string retVal;
    if (0 < m_scopeOfMetaMessages.count(messageIdentifier)) {
        // Get specification for message to be created.
        cluon::MetaMessage message{m_scopeOfMetaMessages[messageIdentifier]};

        // Create "empty" instance for the required message as GenericMessage.
        cluon::GenericMessage gm;
        gm.createFrom(message, m_listOfMetaMessages);

        // Parse data from given JSON.
        std::stringstream sstr{json};
        cluon::FromJSONVisitor jsonDecoder;
        jsonDecoder.decodeFrom(sstr);

        // Set values in the newly created GenericMessage from JSONDecoder.
        gm.accept(jsonDecoder);

        // Finally, transform GenericMessage into Envelope.
        ToProtoVisitor protoEncoder;
        gm.accept(protoEncoder);

        cluon::data::Envelope env;
        env.dataType(messageIdentifier).serializedData(protoEncoder.encodedData());

        retVal = cluon::serializeEnvelope(std::move(env));
    }
    return retVal;
}

} // namespace cluon
