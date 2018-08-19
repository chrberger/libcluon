/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_TOODVDVISITOR_HPP
#define CLUON_TOODVDVISITOR_HPP

#include "cluon/cluon.hpp"

#include <cstdint>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace cluon {
/**
This class provides a visitor to transform a message into its corresponding
message specification in ODVD format:

\code{.cpp}
MyMessage msg;
// Set some values in msg.

cluon::ToODVDVisitor odvd;
msg.accept(odvd);

const std::string generatedMessageSpecification{odvd.messageSpecification()};
std::cout << generatedMessageSpecification << std::endl;

cluon::MessageParser mp;
auto retVal = mp.parse(generatedMessageSpecification);
std::cout << (cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
\endcode
*/
class LIBCLUON_API ToODVDVisitor {
   private:
    ToODVDVisitor(const ToODVDVisitor &) = delete;
    ToODVDVisitor(ToODVDVisitor &&)      = delete;
    ToODVDVisitor &operator=(const ToODVDVisitor &) = delete;
    ToODVDVisitor &operator=(ToODVDVisitor &&) = delete;

   public:
    ToODVDVisitor() = default;

    /**
     * @return Message specification data.
     */
    std::string messageSpecification() const noexcept;

   public:
    // The following methods are provided to allow an instance of this class to
    // be used as visitor for an instance with the method signature void accept<T>(T&);

    void preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept;
    void postVisit() noexcept;

    void visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept;

    template <typename T>
    void visit(uint32_t &id, std::string &&typeName, std::string &&name, T &value) noexcept {
        try {
            std::string tmp{std::regex_replace(typeName, std::regex("::"), ".")}; // NOLINT

            ToODVDVisitor odvdVisitor;
            value.accept(odvdVisitor);
            m_forwardDeclarations.emplace(m_forwardDeclarations.begin(), odvdVisitor.messageSpecification());

            m_buffer << "    " << tmp << ' ' << name << " [ id = " << id << " ];" << '\n';
        } catch (std::regex_error &) { // LCOV_EXCL_LINE
        }
    }

   private:
    std::vector<std::string> m_forwardDeclarations{};
    std::stringstream m_buffer{};
};

} // namespace cluon
#endif
