/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

   public:
    /**
     * This method returns the base64-decoded representation for the given input.
     *
     * @param input to decode from base64
     * @return Decoded input.
     */
    static std::string decodeBase64(const std::string &input) noexcept;

   private:
    std::map<std::string, FromJSONVisitor::JSONKeyValue> readKeyValues(std::string &input) noexcept;

   private:
    std::map<std::string, FromJSONVisitor::JSONKeyValue> m_data{};
    std::map<std::string, FromJSONVisitor::JSONKeyValue> &m_keyValues;
};
} // namespace cluon

#endif
