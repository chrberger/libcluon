/*
 * Copyright (C) 2017  Christian Berger
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

#ifndef ODVDVISITOR_HPP
#define ODVDVISITOR_HPP

#include "cluon/cluon.hpp"

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

cluon::ODVDVisitor odvd;
msg.accept(odvd);

const std::string generatedMessageSpecification{odvd.messageSpecification()};
std::cout << generatedMessageSpecification << std::endl;

cluon::MessageParser mp;
auto retVal = mp.parse(generatedMessageSpecification);
std::cout << (cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
\endcode
*/
class LIBCLUON_API ODVDVisitor {
   private:
    ODVDVisitor(const ODVDVisitor &) = delete;
    ODVDVisitor(ODVDVisitor &&)      = delete;
    ODVDVisitor &operator=(const ODVDVisitor &) = delete;
    ODVDVisitor &operator=(ODVDVisitor &&) = delete;

   public:
    ODVDVisitor() = default;

    /**
     * @return Message specification data.
     */
    std::string messageSpecification() const noexcept;

   public:
    // The following methods are provided to allow an instance of this class to
    // be used as visitor for an instance with the method signature void accept<T>(T&);

    void preVisit(uint32_t id, const std::string &shortName, const std::string &longName) noexcept;
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

            ODVDVisitor odvdVisitor;
            value.accept(odvdVisitor);
            m_forwardDeclarations.emplace(m_forwardDeclarations.begin(), odvdVisitor.messageSpecification());

            m_buffer << "    " << tmp << " " << name << " [ id = " << id << " ];" << '\n';
        } catch (std::regex_error&) { // LCOV_EXCL_LINE
        }
    }

   private:
    std::vector<std::string> m_forwardDeclarations{};
    std::stringstream m_buffer{};
};

} // namespace cluon
#endif
