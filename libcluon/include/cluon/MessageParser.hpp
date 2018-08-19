/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_MESSAGEPARSER_HPP
#define CLUON_MESSAGEPARSER_HPP

#include "cluon/MetaMessage.hpp"
#include "cluon/cluon.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace cluon {
/**
This class provides a parser for message specifications in .odvd format. The
format is inspired by Google Protobuf (https://developers.google.com/protocol-buffers/)
but simplified to enforce backwards and forwards compatibility next to
numerical message identifiers.

This message specification format is also used by OpenDaVINCI (http://code.opendavinci.org).

The parser is based on https://github.com/yhirose/cpp-peglib.

An example for a .odvd compliant message is demonstrated in the following:

\code{.cpp}
const char *spec = R"(
message myMessage.SubName [id = 1] {
    uint8 field1 [id = 1];
    uint32 field2 [id = 2];
    int64 field3 [id = 3];
    string field4 [id = 4];
}
)";

cluon::MessageParser mp;
auto retVal = mp.parse(std::string(spec));
if (retVal.second == cluon::MessageParser::MessageParserErrorCodes::NO_ERROR) {
    auto listOfMessages = retVal.first;
    for (auto message : listOfMessages) {
        message.accept([](const cluon::MetaMessage &mm){ std::cout << "Message name = " << mm.messageName() <<
std::endl; });
    }
}
\endcode
*/
class LIBCLUON_API MessageParser {
   public:
    enum MessageParserErrorCodes : uint8_t { NO_ERROR = 0, SYNTAX_ERROR = 1, DUPLICATE_IDENTIFIERS = 2 };

   private:
    MessageParser(const MessageParser &) = delete;
    MessageParser(MessageParser &&)      = delete;
    MessageParser &operator=(const MessageParser &) = delete;
    MessageParser &operator=(MessageParser &&) = delete;

   public:
    MessageParser() = default;

    /**
     * This method tries to parse the given message specification.
     *
     * @param input Message specification.
     * @return Pair: List of cluon::MetaMessages describing the specified messages and error code:
     *         NO_ERROR: The given specification could be parsed successfully (list moght be non-empty).
     *         SYNTAX_ERROR: The given specification could not be parsed successfully (list is empty).
     *         DUPLICATE_IDENTIFIERS: The given specification contains ambiguous names or identifiers (list is empty).
     */
    std::pair<std::vector<MetaMessage>, MessageParserErrorCodes> parse(const std::string &input);
};
} // namespace cluon

#endif
