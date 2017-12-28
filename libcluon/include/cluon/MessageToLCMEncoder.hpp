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

#ifndef MESSAGEASLCMENCODER_HPP
#define MESSAGEASLCMENCODER_HPP

#include "cluon/cluon.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace cluon {
/**
This class encodes a given message in LCM format.
*/
class LIBCLUON_API MessageToLCMEncoder {
   private:
    MessageToLCMEncoder(const MessageToLCMEncoder &) = delete;
    MessageToLCMEncoder(MessageToLCMEncoder &&)      = delete;
    MessageToLCMEncoder &operator=(const MessageToLCMEncoder &) = delete;
    MessageToLCMEncoder &operator=(MessageToLCMEncoder &&) = delete;

   public:
    MessageToLCMEncoder()  = default;
    ~MessageToLCMEncoder() = default;

    /**
     * @param withHash True if the hash value from the fields shall be included.
     * @return Encoded data in LCM format.
     */
    std::string encodedData(bool withHash = true) const noexcept;

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
        calculateHash(name);
        calculateHash(0);

        // No hash for the type but for name and dimension.
        cluon::MessageToLCMEncoder nestedLCMEncoder;
        value.accept(nestedLCMEncoder);

        constexpr bool WITH_HASH{false};
        const std::string s = nestedLCMEncoder.encodedData(WITH_HASH);
        m_buffer.write(s.c_str(), static_cast<uint32_t>(s.size()));

        // Save this complex field's hash for later to compute final hash.
        m_hashes.push_back(nestedLCMEncoder.hash());
    }

   private:
    int64_t hash() const noexcept;
    void calculateHash(char c) noexcept;
    void calculateHash(const std::string &s) noexcept;

   private:
    int64_t m_hash{0x12345678};
    std::stringstream m_buffer{""};
    std::vector<int64_t> m_hashes{};
};
} // namespace cluon

#endif
