/*
 * Copyright (C) 2017-2018  Christian Berger
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

#ifndef CLUON_FROMLCMVISITOR_HPP
#define CLUON_FROMLCMVISITOR_HPP

#include "cluon/cluon.hpp"

#include <cstdint>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

namespace cluon {
/**
This class decodes a given message from LCM format.
*/
class LIBCLUON_API FromLCMVisitor {
   private:
    FromLCMVisitor(std::stringstream &in) noexcept;
    FromLCMVisitor(const FromLCMVisitor &) = delete;
    FromLCMVisitor(FromLCMVisitor &&)      = delete;
    FromLCMVisitor &operator=(const FromLCMVisitor &) = delete;
    FromLCMVisitor &operator=(FromLCMVisitor &&) = delete;

   public:
    FromLCMVisitor() noexcept;
    ~FromLCMVisitor() = default;

   public:
    /**
     * This method decodes a given istream into LCM.
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
        // No hash for the type but for name and dimension.
        calculateHash(name);
        calculateHash(0);

        cluon::FromLCMVisitor nestedLCMDecoder(m_buffer);
        value.accept(nestedLCMDecoder);

        m_hashes.push_back(nestedLCMDecoder.hash());
    }

   private:
    int64_t hash() const noexcept;
    void calculateHash(char c) noexcept;
    void calculateHash(const std::string &s) noexcept;

   private:
    int64_t m_calculatedHash{0x12345678};
    int64_t m_expectedHash{0};
    std::stringstream m_internalBuffer{""};
    std::stringstream &m_buffer;
    std::vector<int64_t> m_hashes{};
};
} // namespace cluon

#endif
