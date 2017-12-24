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

#include "cluon/ProtoConstants.hpp"
#include "cluon/cluon.hpp"

#include <memory>
#include <sstream>
#include <string>

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
     * @return Encoded data in LCM format.
     */
    std::string encodedData() const noexcept;

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
        (void)name;
        (void)value;

//// TODO: Adjust for LCM.
//        toVarInt(m_buffer, std::move(encodeKey(id, static_cast<uint8_t>(ProtoConstants::LENGTH_DELIMITED))));
//        cluon::MessageToLCMEncoder nestedProtoEncoder;
//        value.accept(nestedProtoEncoder);
//        encode(m_buffer, std::move(nestedProtoEncoder.encodedData()));
    }

   private:
        int64_t hash() const noexcept;
        void calculateHash(char c) noexcept;
        void calculateHash(const std::string &s) noexcept;
        int64_t calculateHash(int64_t v, char c) const noexcept;
        int64_t calculateHash(int64_t v, const std::string &s) const noexcept;

   private:
    int64_t m_hash{0x12345678};
    std::stringstream m_buffer{""};
};
} // namespace cluon

#endif
