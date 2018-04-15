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

#ifndef CLUON_FROMJSONVISITOR_HPP
#define CLUON_FROMJSONVISITOR_HPP

#include "cluon/JSONConstants.hpp"
#include "cluon/any/any.hpp"
#include "cluon/cluon.hpp"

#include <cstdint>
#include <istream>
#include <map>
#include <string>

namespace cluon {
/**
This class decodes a given message from JSON format.
*/
class LIBCLUON_API FromJSONVisitor {
    /**
     * This class represents a key/value in a JSON list of key/values.
     */
    class JSONKeyValue {
       private:
        JSONKeyValue &operator=(JSONKeyValue &&) = delete;

       public:
        JSONKeyValue()                     = default;
        JSONKeyValue(const JSONKeyValue &) = default;
        JSONKeyValue(JSONKeyValue &&)      = default;
        JSONKeyValue &operator=(const JSONKeyValue &) = default;
        ~JSONKeyValue()                               = default;

       public:
        std::string m_key{""};
        JSONConstants m_type{JSONConstants::UNDEFINED};
        linb::any m_value;
    };

   private:
    FromJSONVisitor(const FromJSONVisitor &) = delete;
    FromJSONVisitor(FromJSONVisitor &&)      = delete;
    FromJSONVisitor &operator=(FromJSONVisitor &&) = delete;
    FromJSONVisitor &operator=(const FromJSONVisitor &other) = delete;

    /**
     * Internal constructor to pass reference to preset key/values.
     *
     * @param preset Pre-filled key/value map to handle nested fields.
     */
    FromJSONVisitor(std::map<std::string, FromJSONVisitor::JSONKeyValue> &preset) noexcept;

   public:
    FromJSONVisitor() noexcept;
    ~FromJSONVisitor() = default;

   public:
    /**
     * This method decodes a given istream into an internal key/value representation.
     *
     * @param in istream to decode.
     */
    void decodeFrom(std::istream &in) noexcept;

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
        (void)id;
        (void)typeName;

        if (0 < m_keyValues.count(name)) {
            try {
                std::map<std::string, FromJSONVisitor::JSONKeyValue> v
                    = linb::any_cast<std::map<std::string, FromJSONVisitor::JSONKeyValue>>(m_keyValues[name].m_value);
                cluon::FromJSONVisitor nestedJSONDecoder(v);
                value.accept(nestedJSONDecoder);
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        }
    }

   private:
    std::string decodeBase64(const std::string &input) const noexcept;
    std::map<std::string, FromJSONVisitor::JSONKeyValue> readKeyValues(std::string &input) noexcept;

   private:
    std::map<std::string, FromJSONVisitor::JSONKeyValue> m_data{};
    std::map<std::string, FromJSONVisitor::JSONKeyValue> &m_keyValues;
};
} // namespace cluon

#endif
