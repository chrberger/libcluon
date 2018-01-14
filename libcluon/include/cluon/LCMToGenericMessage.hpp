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

#ifndef LCMTOGENERICMESSAGE_HPP
#define LCMTOGENERICMESSAGE_HPP

#include "cluon/GenericMessage.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/cluon.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace cluon {
/**
This class transforms a given LCM message into a GenericMessage.
*/
class LIBCLUON_API LCMToGenericMessage {
   private:
    LCMToGenericMessage(const LCMToGenericMessage &) = delete;
    LCMToGenericMessage(LCMToGenericMessage &&)      = delete;
    LCMToGenericMessage &operator=(const LCMToGenericMessage &) = delete;
    LCMToGenericMessage &operator=(LCMToGenericMessage &&) = delete;

   public:
    LCMToGenericMessage() = default;

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
     * This method transforms the given LCM payload into a GenericMessage.
     *
     * @param data LCM Payload.
     * @return GenericMessage representation using the given message specification.
     */
    cluon::GenericMessage getGenericMessage(const std::string &data) noexcept;

   private:
    std::vector<cluon::MetaMessage> m_listOfMetaMessages{};
    std::map<std::string, cluon::MetaMessage> m_scopeOfMetaMessages{};
};
} // namespace cluon
#endif
