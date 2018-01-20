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

// clang-format off
#ifdef WIN32
    #include <Winsock2.h> // for htonl, htons
#endif
// clang-format on

#include "cluon/ToMsgPackVisitor.hpp"

#include <cstring>
#include <limits>

namespace cluon {

std::string ToMsgPackVisitor::encodedData() const noexcept {
    std::stringstream prefix;
    if (m_numberOfFields <= 0xF) {
        const uint8_t pairs = static_cast<uint8_t>(MsgPackConstants::FIXMAP) + static_cast<uint8_t>(m_numberOfFields);
        prefix.write(reinterpret_cast<const char *>(&pairs), sizeof(uint8_t));
    } else if ((m_numberOfFields > 0xF) && (m_numberOfFields <= 0xFFFF)) {
        const uint8_t pairs = static_cast<uint8_t>(MsgPackConstants::MAP16);
        prefix.write(reinterpret_cast<const char *>(&pairs), sizeof(uint8_t));
        uint16_t n = htobe16(static_cast<uint16_t>(m_numberOfFields));
        prefix.write(reinterpret_cast<const char *>(&n), sizeof(uint16_t));
    } else if (m_numberOfFields > 0xFFFF) {
        const uint8_t pairs = static_cast<uint8_t>(MsgPackConstants::MAP32);
        prefix.write(reinterpret_cast<const char *>(&pairs), sizeof(uint8_t));
        uint32_t n = htobe32(static_cast<uint32_t>(m_numberOfFields));
        prefix.write(reinterpret_cast<const char *>(&n), sizeof(uint32_t));
    }
    std::string s{prefix.str() + m_buffer.str()};
    return s;
}

void ToMsgPackVisitor::encode(std::ostream &o, const std::string &s) {
    const uint32_t LENGTH{static_cast<uint32_t>(s.size())};
    if (LENGTH < 32) {
        const uint8_t v = static_cast<uint8_t>(MsgPackConstants::FIXSTR) + static_cast<uint8_t>(LENGTH);
        o.write(reinterpret_cast<const char *>(&v), sizeof(uint8_t));
    } else if (LENGTH <= 0xFF) {
        const uint8_t v = static_cast<uint8_t>(MsgPackConstants::STR8);
        o.write(reinterpret_cast<const char *>(&v), sizeof(uint8_t));
        uint8_t l = static_cast<uint8_t>(LENGTH);
        o.write(reinterpret_cast<const char *>(&l), sizeof(uint8_t));
    } else if (LENGTH <= 0xFFFF) {
        const uint8_t v = static_cast<uint8_t>(MsgPackConstants::STR16);
        o.write(reinterpret_cast<const char *>(&v), sizeof(uint8_t));
        uint16_t l = htobe16(static_cast<uint16_t>(LENGTH));
        o.write(reinterpret_cast<const char *>(&l), sizeof(uint16_t));
    } else {
        const uint8_t v = static_cast<uint8_t>(MsgPackConstants::STR32);
        o.write(reinterpret_cast<const char *>(&v), sizeof(uint8_t));
        uint32_t l = htobe32(LENGTH);
        o.write(reinterpret_cast<const char *>(&l), sizeof(uint32_t));
    }
    o.write(s.c_str(), static_cast<std::streamsize>(LENGTH)); // LENGTH won't be negative.
}

void ToMsgPackVisitor::encodeUint(std::ostream &o, uint64_t v) {
    if (0x7f >= v) {
        const uint8_t _v = static_cast<uint8_t>(v);
        o.write(reinterpret_cast<const char *>(&_v), sizeof(uint8_t));
    } else if (0xFF >= v) {
        const uint8_t t = static_cast<uint8_t>(MsgPackConstants::UINT8);
        o.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
        const uint8_t _v = static_cast<uint8_t>(v);
        o.write(reinterpret_cast<const char *>(&_v), sizeof(uint8_t));
    } else if (0xFFFF >= v) {
        const uint8_t t = static_cast<uint8_t>(MsgPackConstants::UINT16);
        o.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
        uint16_t _v = static_cast<uint16_t>(v);
        _v          = htobe16(_v);
        o.write(reinterpret_cast<const char *>(&_v), sizeof(uint16_t));
    } else if (0xFFFFFFFF >= v) {
        const uint8_t t = static_cast<uint8_t>(MsgPackConstants::UINT32);
        o.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
        uint32_t _v = static_cast<uint32_t>(v);
        _v          = htobe32(_v);
        o.write(reinterpret_cast<const char *>(&_v), sizeof(uint32_t));
    } else {
        const uint8_t t = static_cast<uint8_t>(MsgPackConstants::UINT64);
        o.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
        uint64_t _v = v;
        _v          = htobe64(_v);
        o.write(reinterpret_cast<const char *>(&_v), sizeof(uint64_t));
    }
}

void ToMsgPackVisitor::encodeInt(std::ostream &o, int64_t v) {
    if (-31 <= v) {
        int8_t _v = static_cast<int8_t>(v);
        o.write(reinterpret_cast<const char *>(&_v), sizeof(int8_t));
    } else if (std::numeric_limits<int8_t>::lowest() <= v) {
        const uint8_t t = static_cast<uint8_t>(MsgPackConstants::INT8);
        o.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
        int8_t _v = static_cast<int8_t>(v);
        o.write(reinterpret_cast<const char *>(&_v), sizeof(int8_t));
    } else if (std::numeric_limits<int16_t>::lowest() <= v) {
        const uint8_t t = static_cast<uint8_t>(MsgPackConstants::INT16);
        o.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
        int16_t _v = static_cast<int16_t>(v);
        _v         = static_cast<int16_t>(htobe16(_v));
        o.write(reinterpret_cast<const char *>(&_v), sizeof(int16_t));
    } else if (std::numeric_limits<int32_t>::lowest() <= v) {
        const uint8_t t = static_cast<uint8_t>(MsgPackConstants::INT32);
        o.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
        int32_t _v = static_cast<int32_t>(v);
        _v         = static_cast<int32_t>(htobe32(_v));
        o.write(reinterpret_cast<const char *>(&_v), sizeof(int32_t));
    } else {
        const uint8_t t = static_cast<uint8_t>(MsgPackConstants::INT64);
        o.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
        int64_t _v = static_cast<int64_t>(v);
        _v         = static_cast<int64_t>(htobe64(_v));
        o.write(reinterpret_cast<const char *>(&_v), sizeof(int64_t));
    }
}

void ToMsgPackVisitor::preVisit(uint32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;

    m_numberOfFields = 0;
    m_buffer.str("");
}

void ToMsgPackVisitor::postVisit() noexcept {}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    const uint8_t value
        = (v ? static_cast<uint8_t>(MsgPackConstants::IS_TRUE) : static_cast<uint8_t>(MsgPackConstants::IS_FALSE));
    m_buffer.write(reinterpret_cast<const char *>(&value), sizeof(uint8_t));
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    const std::string s(1, v);
    encode(m_buffer, s);
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    encodeInt(m_buffer, v);
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    encodeUint(m_buffer, v);
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    encodeInt(m_buffer, v);
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    encodeUint(m_buffer, v);
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    encodeInt(m_buffer, v);
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    encodeUint(m_buffer, v);
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    encodeInt(m_buffer, v);
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    encodeUint(m_buffer, v);
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    const uint8_t t = static_cast<uint8_t>(MsgPackConstants::FLOAT);
    m_buffer.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
    uint32_t _v{0};
    std::memmove(&_v, &v, sizeof(float));
    _v = htobe32(_v);
    m_buffer.write(reinterpret_cast<const char *>(&_v), sizeof(uint32_t));
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    const uint8_t t = static_cast<uint8_t>(MsgPackConstants::DOUBLE);
    m_buffer.write(reinterpret_cast<const char *>(&t), sizeof(uint8_t));
    uint64_t _v{0};
    std::memmove(&_v, &v, sizeof(double));
    _v = htobe64(_v);
    m_buffer.write(reinterpret_cast<const char *>(&_v), sizeof(double));
    m_numberOfFields++;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;

    encode(m_buffer, name);
    encode(m_buffer, v);
    m_numberOfFields++;
}

} // namespace cluon
