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

#include <sstream>
#include <string>

namespace cluon {
/**
This class provides a visitor to transform a message into CSV with
user-specified delimiters and optional column headers:

\code{.cpp}
MyMessage msg;
// Set some values in msg.

cluon::CSVVisitor csv{',', true};
msg.accept(csv);

std::cout << csv.csv() << std::endl;
\endcode

Subsequent use of this visitor will append the data (please keep in mind to not
change the visited messages in between as the generated CSV data will be messed
up otherwise).
*/
class LIBCLUON_API CSVVisitor {
   private:
    CSVVisitor(const CSVVisitor &) = delete;
    CSVVisitor(CSVVisitor &&)      = delete;
    CSVVisitor &operator=(const CSVVisitor &) = delete;
    CSVVisitor &operator=(CSVVisitor &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param prefix Prefix to prepend per column header.
     * @param delimiter Delimiter character.
     * @param withHeader If true, the first line in the output contains the
     *        column headers.
     * @param isNested If true, the returned CSV values do not have a trailing new line.
     */
    CSVVisitor(char delimiter = ';', bool withHeader = true) noexcept;

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
    CSVVisitor(const std::string &prefix, char delimiter, bool withHeader, bool isNested) noexcept;

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
        (void)id;
        (void)typeName;
        constexpr bool IS_NESTED{true};
        CSVVisitor csvVisitor(name, m_delimiter, m_withHeader, IS_NESTED);
        value.accept(csvVisitor);

        if (m_fillHeader) {
            m_bufferHeader << csvVisitor.m_bufferHeader.str();
        }
        m_bufferValues << csvVisitor.m_bufferValues.str();
    }

   private:
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
