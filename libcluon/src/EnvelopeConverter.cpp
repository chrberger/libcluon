/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/EnvelopeConverter.hpp"
#include "cluon/Envelope.hpp"
#include "cluon/FromJSONVisitor.hpp"
#include "cluon/FromProtoVisitor.hpp"
#include "cluon/GenericMessage.hpp"
#include "cluon/MessageParser.hpp"
#include "cluon/Time.hpp"
#include "cluon/ToJSONVisitor.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/any/any.hpp"

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
    if (cluon::MessageParser::MessageParserErrorCodes::NO_MESSAGEPARSER_ERROR == parsingResult.second) {
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
            // Directly decoding complete OD4 container failed, try decoding without header.
            cluon::FromProtoVisitor protoDecoder;
            protoDecoder.decodeFrom(sstr, envelope);
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
            try {
                // Catch possible linb::any exception.
                gm.accept(payloadToJSON);
            } catch (const linb::bad_any_cast &) {} // LCOV_EXCL_LINE

            std::string tmp{payload.messageName()};
            std::replace(tmp.begin(), tmp.end(), '.', '_');

            const std::string strPayloadJSON{payloadToJSON.json() != "{}" ? payloadToJSON.json() : ""};

            retVal = '{' + envelopeToJSON.json() + ',' + '\n' + '"' + tmp + '"' + ':' + '{' + strPayloadJSON + '}' + '}';
        }
    }
    return retVal;
}

// clang-format off
std::string EnvelopeConverter::getProtoEncodedEnvelopeFromJSONWithoutTimeStamps(const std::string &json, int32_t messageIdentifier, uint32_t senderStamp) noexcept {
    // clang-format on
    return getProtoEncodedEnvelopeFromJSON(json, messageIdentifier, senderStamp, cluon::data::TimeStamp());
}

// clang-format off
std::string EnvelopeConverter::getProtoEncodedEnvelopeFromJSON(const std::string &json, int32_t messageIdentifier, uint32_t senderStamp) noexcept {
    // clang-format on
    return getProtoEncodedEnvelopeFromJSON(json, messageIdentifier, senderStamp, cluon::time::now());
}

// clang-format off
std::string EnvelopeConverter::getProtoEncodedEnvelopeFromJSON(const std::string &json, int32_t messageIdentifier, uint32_t senderStamp, cluon::data::TimeStamp sampleTimeStamp) noexcept {
    // clang-format on
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
        env.dataType(messageIdentifier).serializedData(protoEncoder.encodedData()).senderStamp(senderStamp).sampleTimeStamp(sampleTimeStamp);

        retVal = cluon::serializeEnvelope(std::move(env));
    }
    return retVal;
}

} // namespace cluon
