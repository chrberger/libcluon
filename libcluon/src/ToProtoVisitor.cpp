/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/ToProtoVisitor.hpp"

#include <cstring>

namespace cluon {

std::string ToProtoVisitor::encodedData() const noexcept {
    std::string s{m_buffer.str()};
    return s;
}

void ToProtoVisitor::preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void ToProtoVisitor::postVisit() noexcept {}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    (void)name;
    toKeyValue<bool>(id, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    (void)name;
    uint8_t _v = static_cast<uint8_t>(v); // NOLINT
    toKeyValue<uint8_t>(id, _v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    (void)name;
    toKeyValue<int8_t>(id, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    (void)name;
    toKeyValue<uint8_t>(id, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    (void)name;
    toKeyValue<int16_t>(id, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    (void)name;
    toKeyValue<uint16_t>(id, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    (void)name;
    toKeyValue<int32_t>(id, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    (void)name;
    toKeyValue<uint32_t>(id, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    (void)name;
    toKeyValue<int64_t>(id, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    (void)name;
    toKeyValue<uint64_t>(id, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    (void)name;
    uint64_t key = encodeKey(id, static_cast<uint8_t>(ProtoConstants::FOUR_BYTES));
    toVarInt(m_buffer, key);
    encode(m_buffer, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    (void)name;
    uint64_t key = encodeKey(id, static_cast<uint8_t>(ProtoConstants::EIGHT_BYTES));
    toVarInt(m_buffer, key);
    encode(m_buffer, v);
}

void ToProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    (void)name;
    uint64_t key = encodeKey(id, static_cast<uint8_t>(ProtoConstants::LENGTH_DELIMITED));
    toVarInt(m_buffer, key);
    encode(m_buffer, v);
}

////////////////////////////////////////////////////////////////////////////////

std::size_t ToProtoVisitor::encode(std::ostream &o, bool &v) noexcept {
    uint64_t _v{(v ? 1u : 0u)};
    return toVarInt(o, _v);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, int8_t &v) noexcept {
    uint64_t _v = toZigZag8(v);
    return toVarInt(o, _v);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, uint8_t &v) noexcept {
    uint64_t _v = v;
    return toVarInt(o, _v);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, int16_t &v) noexcept {
    uint64_t _v = toZigZag16(v);
    return toVarInt(o, _v);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, uint16_t &v) noexcept {
    uint64_t _v = v;
    return toVarInt(o, _v);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, int32_t &v) noexcept {
    uint64_t _v = toZigZag32(v);
    return toVarInt(o, _v);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, uint32_t &v) noexcept {
    uint64_t _v = v;
    return toVarInt(o, _v);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, int64_t &v) noexcept {
    uint64_t _v = toZigZag64(v);
    return toVarInt(o, _v);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, uint64_t &v) noexcept {
    return toVarInt(o, v);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, float &v) noexcept {
    // Store 4 bytes as little endian encoding.
    uint32_t _v{0};
    std::memmove(&_v, &v, sizeof(float));
    _v = htole32(_v);
    o.write(reinterpret_cast<const char *>(&_v), sizeof(uint32_t)); // NOLINT
    return sizeof(uint32_t);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, double &v) noexcept {
    // Store 8 bytes as little endian encoding.
    uint64_t _v{0};
    std::memmove(&_v, &v, sizeof(double));
    _v = htole64(_v);
    o.write(reinterpret_cast<const char *>(&_v), sizeof(uint64_t)); // NOLINT
    return sizeof(uint64_t);
}

std::size_t ToProtoVisitor::encode(std::ostream &o, const std::string &v) noexcept {
    const std::size_t LENGTH = v.length();
    std::size_t size         = toVarInt(o, LENGTH);
    o.write(v.c_str(), static_cast<std::streamsize>(LENGTH)); // LENGTH won't be negative.
    return size + LENGTH;
}

uint8_t ToProtoVisitor::toZigZag8(int8_t v) noexcept {
    return static_cast<uint8_t>((v << 1) ^ (v >> ((sizeof(v) * 8) - 1)));
}

uint16_t ToProtoVisitor::toZigZag16(int16_t v) noexcept {
    return static_cast<uint16_t>((v << 1) ^ (v >> ((sizeof(v) * 8) - 1)));
}

uint32_t ToProtoVisitor::toZigZag32(int32_t v) noexcept {
    return static_cast<uint32_t>((v << 1) ^ (v >> ((sizeof(v) * 8) - 1)));
}

uint64_t ToProtoVisitor::toZigZag64(int64_t v) noexcept {
    return static_cast<uint64_t>((v << 1) ^ (v >> ((sizeof(v) * 8) - 1)));
}

uint64_t ToProtoVisitor::encodeKey(uint32_t fieldIdentifier, uint8_t protoType) noexcept {
    return (fieldIdentifier << 0x3) | protoType;
}

std::size_t ToProtoVisitor::toVarInt(std::ostream &out, uint64_t v) noexcept {
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
