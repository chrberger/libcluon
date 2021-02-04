/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_TOCSVVISITOR_HPP
#define CLUON_TOCSVVISITOR_HPP

#include "cluon/cluon.hpp"

#include <cstdint>
#include <map>
#include <sstream>
#include <string>

namespace cluon {
/**
This class provides a visitor to transform a message into CSV with
user-specified delimiters and optional column headers:

\code{.cpp}
MyMessage msg;
// Set some values in msg.

cluon::ToCSVVisitor csv{',', true};
msg.accept(csv);

std::cout << csv.csv() << std::endl;
\endcode

Subsequent use of this visitor will append the data (please keep in mind to not
change the visited messages in between as the generated CSV data will be messed
up otherwise).
*/
class LIBCLUON_API ToCSVVisitor {
   private:
    ToCSVVisitor(const ToCSVVisitor &) = delete;
    ToCSVVisitor(ToCSVVisitor &&)      = delete;
    ToCSVVisitor &operator=(const ToCSVVisitor &) = delete;
    ToCSVVisitor &operator=(ToCSVVisitor &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param delimiter Delimiter character.
     * @param withHeader If true, the first line in the output contains the
     *        column headers.
     * @param mask Map describing which fields to render. If empty, all
     *             fields will be emitted; individual field identifiers
     *             can be masked setting them to false.
     */
    ToCSVVisitor(char delimiter = ';', bool withHeader = true, const std::map<uint32_t, bool> &mask = {}) noexcept;

   protected:
    /**
     * Constructor for internal use.
     *
     * @param prefix Prefix to prepend per column header.
     * @param delimiter Delimiter character.
     * @param withHeader If true, the first line in the output contains the
     *        column headers.
     * @param isNested If true, the returned CSV values do not have a trailing new line.
     */
    ToCSVVisitor(const std::string &prefix, char delimiter, bool withHeader, bool isNested) noexcept;

   public:
    /**
     * @return CSV-encoded data.
     */
    std::string csv() const noexcept;

    /**
     * This method clears the containing CSV data.
     */
    void clear() noexcept;

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
        if ((0 == m_mask.count(id)) || m_mask[id]) {
            constexpr bool IS_NESTED{true};
            ToCSVVisitor csvVisitor(name, m_delimiter, m_withHeader, IS_NESTED);
            value.accept(csvVisitor);

            if (m_fillHeader) {
                m_bufferHeader << csvVisitor.m_bufferHeader.str();
            }
            m_bufferValues << csvVisitor.m_bufferValues.str();
        }
    }

   private:
    std::map<uint32_t, bool> m_mask{};
    std::string m_prefix{};
    char m_delimiter{';'};
    bool m_withHeader{true};
    bool m_isNested{false};
    bool m_fillHeader{true};
    std::stringstream m_bufferHeader{};
    std::stringstream m_bufferValues{};
};

} // namespace cluon
#endif
