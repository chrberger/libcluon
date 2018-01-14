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

#ifndef JSONVISITOR_HPP
#define JSONVISITOR_HPP

#include "cluon/cluon.hpp"

#include <cstdint>
#include <map>
#include <sstream>
#include <string>

namespace cluon {
/**
This class provides a visitor to transform a message into JSON:

\code{.cpp}
MyMessage msg;
// Set some values in msg.

cluon::JSONVisitor j;
msg.accept(j);

std::cout << j.json() << std::endl;
\endcode
*/
class LIBCLUON_API JSONVisitor {
   private:
    JSONVisitor(const JSONVisitor &) = delete;
    JSONVisitor(JSONVisitor &&)      = delete;
    JSONVisitor &operator=(const JSONVisitor &) = delete;
    JSONVisitor &operator=(JSONVisitor &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param withOuterCurlyBraces Include the outer curly braces.
     * @param mask Map describing which fields to render. If empty, all
     *             fields will be emitted; individual field identifiers
     *             can be masked setting them to false.
     */
    JSONVisitor(bool withOuterCurlyBraces = true, const std::map<uint32_t, bool> &mask = {}) noexcept;

    /**
     * @return JSON-encoded data.
     */
    std::string json() const noexcept;

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
        (void)typeName;
        if ((0 == m_mask.count(id)) || m_mask[id]) {
            JSONVisitor jsonVisitor;
            value.accept(jsonVisitor);
            m_buffer << '\"' << name << '\"' << ':' << jsonVisitor.json() << ',' << '\n';
        }
    }

    /**
     * This method returns the base64-encoded representation for the given input.
     *
     * @param input to encode as base64
     * @return base64 encoded input.
     */
    std::string encodeBase64(const std::string &input) const noexcept;

   private:
    bool m_withOuterCurlyBraces{true};
    std::map<uint32_t, bool> m_mask;
    std::stringstream m_buffer{};
};

} // namespace cluon
#endif
