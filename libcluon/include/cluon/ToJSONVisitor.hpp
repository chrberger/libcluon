/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_TOJSONVISITOR_HPP
#define CLUON_TOJSONVISITOR_HPP

#include "cluon/any/any.hpp"
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

cluon::ToJSONVisitor j;
msg.accept(j);

std::cout << j.json() << std::endl;
\endcode
*/
class LIBCLUON_API ToJSONVisitor {
   private:
    ToJSONVisitor(const ToJSONVisitor &) = delete;
    ToJSONVisitor(ToJSONVisitor &&)      = delete;
    ToJSONVisitor &operator=(const ToJSONVisitor &) = delete;
    ToJSONVisitor &operator=(ToJSONVisitor &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param withOuterCurlyBraces Include the outer curly braces.
     * @param mask Map describing which fields to render. If empty, all
     *             fields will be emitted; individual field identifiers
     *             can be masked setting them to false.
     */
    ToJSONVisitor(bool withOuterCurlyBraces = true, const std::map<uint32_t, bool> &mask = {}) noexcept;

    /**
     * @return JSON-encoded data.
     */
    std::string json() const noexcept;

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
        (void)typeName;
        if ((0 == m_mask.count(id)) || m_mask[id]) {
            try {
                ToJSONVisitor jsonVisitor;
                value.accept(jsonVisitor);
                m_buffer << '\"' << name << '\"' << ':' << jsonVisitor.json() << ',' << '\n';
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        }
    }

   public:
    /**
     * This method returns the base64-encoded representation for the given input.
     *
     * @param input to encode as base64
     * @return base64 encoded input.
     */
    static std::string encodeBase64(const std::string &input) noexcept;

   private:
    bool m_withOuterCurlyBraces{true};
    std::map<uint32_t, bool> m_mask;
    std::stringstream m_buffer{};
};

} // namespace cluon
#endif
