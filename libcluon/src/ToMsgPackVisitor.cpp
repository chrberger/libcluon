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

namespace cluon {

std::string ToMsgPackVisitor::encodedData() const noexcept {
    std::stringstream prefix;
    const uint8_t pairs = (m_numberOfFields <= 0xF ? static_cast<uint8_t>(MsgPackConstants::FIXMAP) + static_cast<uint8_t>(m_numberOfFields) :
                            (m_numberOfFields <= 0xFFFF ? static_cast<uint8_t>(MsgPackConstants::MAP16) : static_cast<uint8_t>(MsgPackConstants::MAP32))
                        );
    prefix.write(reinterpret_cast<const char*>(&pairs), sizeof(uint8_t));
    if ( (m_numberOfFields > 0xF) && (m_numberOfFields <= 0xFFFF) ) {
        uint16_t n = htobe16(static_cast<uint16_t>(m_numberOfFields));
        prefix.write(reinterpret_cast<const char*>(&n), sizeof(uint16_t));
    }
    else if (m_numberOfFields > 0xFFFF) {
        uint32_t n = htobe32(static_cast<uint32_t>(m_numberOfFields));
        prefix.write(reinterpret_cast<const char*>(&n), sizeof(uint32_t));
    }
    std::string s{prefix.str() + m_buffer.str()};
    return s;
}

void ToMsgPackVisitor::encode(std::ostream &o, const std::string &s) {
    const uint32_t LENGTH{static_cast<uint32_t>(s.size())};
    const uint8_t v = (LENGTH < 32 ? static_cast<uint8_t>(MsgPackConstants::FIXSTR) + static_cast<uint8_t>(LENGTH) :
                          (LENGTH <= 0xFF ? static_cast<uint8_t>(MsgPackConstants::STR8) : 
                            (LENGTH <= 0xFFFF ? static_cast<uint8_t>(MsgPackConstants::STR16) : static_cast<uint8_t>(MsgPackConstants::STR32))
                          )
                        );
    m_buffer.write(reinterpret_cast<const char*>(&v), sizeof(uint8_t));
    if ( (LENGTH >= 32) && (LENGTH <= 0xFF) ) {
        uint16_t l = htobe16(static_cast<uint16_t>(LENGTH));
        m_buffer.write(reinterpret_cast<const char*>(&l), sizeof(uint16_t));
    }
    else if ( (LENGTH > 0xFF) && (LENGTH <= 0xFF) ) {
        uint32_t l = htobe32(LENGTH);
        m_buffer.write(reinterpret_cast<const char*>(&l), sizeof(uint32_t));
    }
    m_buffer.write(s.c_str(), static_cast<std::streamsize>(LENGTH)); // LENGTH won't be negative.
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
    uint8_t value = (v ? static_cast<uint8_t>(MsgPackConstants::IS_TRUE) : static_cast<uint8_t>(MsgPackConstants::IS_FALSE));
    m_buffer.write(reinterpret_cast<const char*>(&value), sizeof(uint8_t));
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

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

void ToMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;

    (void)name;
    (void)v;
}

} // namespace cluon
