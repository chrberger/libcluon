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

#include "cluon/MessageToProtoEncoder.hpp"

#include <cstring>

namespace cluon {

std::string MessageToProtoEncoder::encodedData() const noexcept {
    std::string s{m_protoBuffer.str()};
    return s;
}

////////////////////////////////////////////////////////////////////////////////

void MessageToProtoEncoder::visit(uint32_t id, bool &v) noexcept {
    toKeyValue<bool>(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, char &v) noexcept {
    uint8_t _v = static_cast<uint8_t>(v); // NOLINT
    toKeyValue<uint8_t>(id, _v);
}

void MessageToProtoEncoder::visit(uint32_t id, int8_t &v) noexcept {
    toKeyValue<int8_t>(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, uint8_t &v) noexcept {
    toKeyValue<uint8_t>(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, int16_t &v) noexcept {
    toKeyValue<int16_t>(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, uint16_t &v) noexcept {
    toKeyValue<uint16_t>(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, int32_t &v) noexcept {
    toKeyValue<int32_t>(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, uint32_t &v) noexcept {
    toKeyValue<uint32_t>(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, int64_t &v) noexcept {
    toKeyValue<int64_t>(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, uint64_t &v) noexcept {
    toKeyValue<uint64_t>(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, float &v) noexcept {
    uint64_t key = encodeKey(id, static_cast<uint8_t>(ProtoConstants::FOUR_BYTES));
    toVarInt(m_protoBuffer, key);
    encode(m_protoBuffer, v);
}

void MessageToProtoEncoder::visit(uint32_t id, double &v) noexcept {
    uint64_t key = encodeKey(id, static_cast<uint8_t>(ProtoConstants::EIGHT_BYTES));
    toVarInt(m_protoBuffer, key);
    encode(m_protoBuffer, v);
}

void MessageToProtoEncoder::visit(uint32_t id, const std::string &v) noexcept {
    uint64_t key = encodeKey(id, static_cast<uint8_t>(ProtoConstants::LENGTH_DELIMITED));
    toVarInt(m_protoBuffer, key);
    encode(m_protoBuffer, v);
}

////////////////////////////////////////////////////////////////////////////////

void MessageToProtoEncoder::preVisit(uint32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void MessageToProtoEncoder::postVisit() noexcept {}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageToProtoEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

////////////////////////////////////////////////////////////////////////////////

std::size_t MessageToProtoEncoder::encode(std::ostream &o, bool &v) noexcept {
    uint64_t _v{(v ? 1u : 0u)};
    return toVarInt(o, _v);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, int8_t &v) noexcept {
    uint64_t _v = toZigZag8(v);
    return toVarInt(o, _v);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, uint8_t &v) noexcept {
    uint64_t _v = v;
    return toVarInt(o, _v);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, int16_t &v) noexcept {
    uint64_t _v = toZigZag16(v);
    return toVarInt(o, _v);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, uint16_t &v) noexcept {
    uint64_t _v = v;
    return toVarInt(o, _v);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, int32_t &v) noexcept {
    uint64_t _v = toZigZag32(v);
    return toVarInt(o, _v);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, uint32_t &v) noexcept {
    uint64_t _v = v;
    return toVarInt(o, _v);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, int64_t &v) noexcept {
    uint64_t _v = toZigZag64(v);
    return toVarInt(o, _v);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, uint64_t &v) noexcept {
    return toVarInt(o, v);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, float &v) noexcept {
    // Store 4 bytes as little endian encoding.
    uint32_t _v{0};
    std::memmove(&_v, &v, sizeof(float));
    _v = htole32(_v);
    o.write(reinterpret_cast<const char *>(&_v), sizeof(uint32_t)); // NOLINT
    return sizeof(uint32_t);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, double &v) noexcept {
    // Store 8 bytes as little endian encoding.
    uint64_t _v{0};
    std::memmove(&_v, &v, sizeof(double));
    _v = htole64(_v);
    o.write(reinterpret_cast<const char *>(&_v), sizeof(uint64_t)); // NOLINT
    return sizeof(uint64_t);
}

std::size_t MessageToProtoEncoder::encode(std::ostream &o, const std::string &v) noexcept {
    const std::size_t LENGTH = v.length();
    std::size_t size         = toVarInt(o, LENGTH);
    o.write(v.c_str(), static_cast<std::streamsize>(LENGTH)); // LENGTH won't be negative.
    return size + LENGTH;
}

uint8_t MessageToProtoEncoder::toZigZag8(int8_t v) noexcept {
    return static_cast<uint8_t>((v << 1) ^ (v >> ((sizeof(v) * 8) - 1)));
}

uint16_t MessageToProtoEncoder::toZigZag16(int16_t v) noexcept {
    return static_cast<uint16_t>((v << 1) ^ (v >> ((sizeof(v) * 8) - 1)));
}

uint32_t MessageToProtoEncoder::toZigZag32(int32_t v) noexcept {
    return static_cast<uint32_t>((v << 1) ^ (v >> ((sizeof(v) * 8) - 1)));
}

uint64_t MessageToProtoEncoder::toZigZag64(int64_t v) noexcept {
    return static_cast<uint64_t>((v << 1) ^ (v >> ((sizeof(v) * 8) - 1)));
}

uint64_t MessageToProtoEncoder::encodeKey(uint32_t fieldIdentifier, uint8_t protoType) noexcept {
    return (fieldIdentifier << 0x3) | protoType;
}

std::size_t MessageToProtoEncoder::toVarInt(std::ostream &out, uint64_t v) noexcept {
    // VarInt is little endian.
    v = htole64(v);

    // Minimum size is of the encoded data.
    std::size_t size{1};
    uint8_t b{0};
    while (0x7f < v) {
        // Use the MSB to indicate value overflow for more bytes to come.
        b = (static_cast<uint8_t>(v & 0x7f)) | 0x80;
        out.put(static_cast<char>(b));
        v >>= 7;
        size++;
    }
    // Write final byte.
    b = (static_cast<uint8_t>(v)) & 0x7f;
    out.put(static_cast<char>(b));

    return size;
}
} // namespace cluon
