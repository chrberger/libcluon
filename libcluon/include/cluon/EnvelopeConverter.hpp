/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_ENVELOPECONVERTER_HPP
#define CLUON_ENVELOPECONVERTER_HPP

#include "cluon/MetaMessage.hpp"
#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace cluon {
/**
This class provides various conversion functions to and from Envelope data structures.
*/
class LIBCLUON_API EnvelopeConverter {
   private:
    EnvelopeConverter(const EnvelopeConverter &) = delete;
    EnvelopeConverter(EnvelopeConverter &&)      = delete;
    EnvelopeConverter &operator=(const EnvelopeConverter &) = delete;
    EnvelopeConverter &operator=(EnvelopeConverter &&) = delete;

   public:
    EnvelopeConverter() = default;

    /**
     * This method sets the message specification to be used for
     * interpreting a given Proto-encoded Envelope.
     *
     * @param ms Message specification following the ODVD format.
     * @return -1 in case of invalid message specification; otherwise, number
     *         of successfully parsed messages from given message specification.
     */
    int32_t setMessageSpecification(const std::string &ms) noexcept;

    /**
     * This method transforms the given Proto-encoded Envelope to JSON. The
     * Proto-encoded envelope might be preceded with a 5-bytes OD4-header (optional).
     *
     * @param protoEncodedEnvelope Proto-encoded Envelope.
     * @return JSON representation from given Proto-encoded Envelope using the
     *         given message specification.
     */
    std::string getJSONFromProtoEncodedEnvelope(const std::string &protoEncodedEnvelope) noexcept;

    /**
     * This method transforms the given Envelope to JSON.
     *
     * @param envelope Envelope.
     * @return JSON representation from given Envelope using the given message specification.
     */
    std::string getJSONFromEnvelope(cluon::data::Envelope &envelope) noexcept;

    /**
     * This method transforms a given JSON representation into a Proto-encoded Envelope
     * including the prepended OD4-header.
     *
     * @param json representation according to the given message specification.
     * @param messageIdentifier The given JSON representation shall be interpreted
     *        as the specified message.
     * @param senderStamp to be used in the Envelope.
     * @return Proto-encoded Envelope including OD4-header or empty string.
     */
    std::string getProtoEncodedEnvelopeFromJSONWithoutTimeStamps(const std::string &json, int32_t messageIdentifier, uint32_t senderStamp) noexcept;

    /**
     * This method transforms a given JSON representation into a Proto-encoded Envelope
     * including the prepended OD4-header and setting cluon::time::now() as sampleTimeStamp.
     *
     * @param json representation according to the given message specification.
     * @param messageIdentifier The given JSON representation shall be interpreted
     *        as the specified message.
     * @param senderStamp to be used in the Envelope.
     * @return Proto-encoded Envelope including OD4-header or empty string.
     */
    std::string getProtoEncodedEnvelopeFromJSON(const std::string &json, int32_t messageIdentifier, uint32_t senderStamp) noexcept;

   private:
    std::string getProtoEncodedEnvelopeFromJSON(const std::string &json, int32_t messageIdentifier, uint32_t senderStamp, cluon::data::TimeStamp sampleTimeStamp) noexcept;

   private:
    std::vector<cluon::MetaMessage> m_listOfMetaMessages{};
    std::map<int32_t, cluon::MetaMessage> m_scopeOfMetaMessages{};
};
} // namespace cluon
#endif
