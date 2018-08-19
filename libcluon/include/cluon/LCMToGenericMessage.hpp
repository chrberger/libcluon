/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_LCMTOGENERICMESSAGE_HPP
#define CLUON_LCMTOGENERICMESSAGE_HPP

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
     * interpreting a given LCM-encoded message.
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
