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

#ifndef MESSAGEFROMLCMDECODER_HPP
#define MESSAGEFROMLCMDECODER_HPP

#include "cluon/cluon.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace cluon {
/**
This class decodes a given message from LCM format.
*/
class LIBCLUON_API MessageFromLCMDecoder {
   private:
    MessageFromLCMDecoder(const MessageFromLCMDecoder &) = delete;
    MessageFromLCMDecoder(MessageFromLCMDecoder &&)      = delete;
    MessageFromLCMDecoder &operator=(const MessageFromLCMDecoder &) = delete;
    MessageFromLCMDecoder &operator=(MessageFromLCMDecoder &&) = delete;

   public:
    MessageFromLCMDecoder()  = default;
    ~MessageFromLCMDecoder() = default;

   public:
    /**
     * This method decodes a given istream into LCM.
     *
     * @param in istream to decode.
     * @param hasHash true if the stream contains a hash value from the fields.
     */
    void decodeFrom(std::istream &in, bool hasHash = true) noexcept;

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
        // No hash for the type but for name and dimension.
        calculateHash(name);
        calculateHash(0);

        // Simply copy the remaining read buffer over.
        std::stringstream buffer;
        char c{0};
        while (m_buffer.good()) {
            c = m_buffer.get();
            buffer.put(c);
        }

        constexpr bool HAS_HASH{false};
        cluon::MessageFromLCMDecoder nestedLCMDecoder;
        nestedLCMDecoder.decodeFrom(buffer, HAS_HASH);

        value.accept(nestedLCMDecoder);

        m_calculatedHash += nestedLCMDecoder.hash();
    }

   private:
    int64_t hash() const noexcept;
    void calculateHash(char c) noexcept;
    void calculateHash(const std::string &s) noexcept;

    float ntohf(float f) const noexcept;
    double ntohd(double d) const noexcept;

   private:
    int64_t m_calculatedHash{0x12345678};
    int64_t m_expectedHash{0};
    std::stringstream m_buffer{""};
};
} // namespace cluon

#endif
