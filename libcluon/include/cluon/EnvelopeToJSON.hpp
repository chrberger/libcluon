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

#ifndef ENVELOPETOJSON_HPP
#define ENVELOPETOJSON_HPP

#include "cluon/MetaMessage.hpp"
#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace cluon {
/**
This class transforms a given Envelope into a JSON representation for its
contained payload.
*/
class LIBCLUON_API EnvelopeToJSON {
   private:
    EnvelopeToJSON(const EnvelopeToJSON &) = delete;
    EnvelopeToJSON(EnvelopeToJSON &&)      = delete;
    EnvelopeToJSON &operator=(const EnvelopeToJSON &) = delete;
    EnvelopeToJSON &operator=(EnvelopeToJSON &&) = delete;

   public:
    EnvelopeToJSON() = default;

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

   private:
    std::vector<cluon::MetaMessage> m_listOfMetaMessages{};
    std::map<uint32_t, cluon::MetaMessage> m_scopeOfMetaMessages{};
};
} // namespace cluon
#endif
