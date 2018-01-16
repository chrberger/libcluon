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

#ifndef MESSAGEASPROTOENCODER_HPP
#define MESSAGEASPROTOENCODER_HPP

#include "cluon/ProtoConstants.hpp"
#include "cluon/cluon.hpp"

#include <cstdint>
#include <sstream>
#include <string>

namespace cluon {
/**
This class encodes a given message in Proto format.
*/
class LIBCLUON_API MessageToProtoEncoder {
   private:
    MessageToProtoEncoder(const MessageToProtoEncoder &) = delete;
    MessageToProtoEncoder(MessageToProtoEncoder &&)      = delete;
    MessageToProtoEncoder &operator=(const MessageToProtoEncoder &) = delete;
    MessageToProtoEncoder &operator=(MessageToProtoEncoder &&) = delete;

   public:
    MessageToProtoEncoder()  = default;
    ~MessageToProtoEncoder() = default;

    /**
     * @return Encoded data in Proto format.
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
        (void)typeName;
        (void)name;

        toVarInt(m_buffer, std::move(encodeKey(id, static_cast<uint8_t>(ProtoConstants::LENGTH_DELIMITED))));
        cluon::MessageToProtoEncoder nestedProtoEncoder;
        value.accept(nestedProtoEncoder);
        encode(m_buffer, std::move(nestedProtoEncoder.encodedData()));
    }

   private:
    std::size_t encode(std::ostream &o, bool &v) noexcept;
    std::size_t encode(std::ostream &o, int8_t &v) noexcept;
    std::size_t encode(std::ostream &o, uint8_t &v) noexcept;
    std::size_t encode(std::ostream &o, int16_t &v) noexcept;
    std::size_t encode(std::ostream &o, uint16_t &v) noexcept;
    std::size_t encode(std::ostream &o, int32_t &v) noexcept;
    std::size_t encode(std::ostream &o, uint32_t &v) noexcept;
    std::size_t encode(std::ostream &o, int64_t &v) noexcept;
    std::size_t encode(std::ostream &o, uint64_t &v) noexcept;
    std::size_t encode(std::ostream &o, float &v) noexcept;
    std::size_t encode(std::ostream &o, double &v) noexcept;
    std::size_t encode(std::ostream &o, const std::string &v) noexcept;

   private:
    uint8_t toZigZag8(int8_t v) noexcept;
    uint16_t toZigZag16(int16_t v) noexcept;
    uint32_t toZigZag32(int32_t v) noexcept;
    uint64_t toZigZag64(int64_t v) noexcept;

    /**
     * This method encodes a given value in VarInt.
     *
     * @param out std::ostream to encode to.
     * @param v Value to encode.
     * @return Bytes written.
     */
    std::size_t toVarInt(std::ostream &out, uint64_t v) noexcept;

    /**
     * This method creates a key/value pair encoded in Proto format.
     *
     * @param fieldIdentifier Field identifier.
     * @param v Value to encode.
     * @return Bytes written.
     */
    template <typename T>
    std::size_t toKeyValue(uint32_t fieldIdentifier, T &v) noexcept {
        std::size_t size{0};
        uint64_t key = encodeKey(fieldIdentifier, static_cast<uint8_t>(ProtoConstants::VARINT));
        size += toVarInt(m_buffer, key);
        size += encode(m_buffer, v);
        return size;
    }

    /**
     * This method creates the key for the key/value pair in Protobuf format.
     *
     * @param fieldIdentifier Field identifier.
     * @param protoType Protobuf type identifier.
     * @return Protobuf fieldIdentifier/key pair.
     */
    uint64_t encodeKey(uint32_t fieldIdentifier, uint8_t protoType) noexcept;

   private:
    std::stringstream m_buffer{""};
};
} // namespace cluon

#endif
