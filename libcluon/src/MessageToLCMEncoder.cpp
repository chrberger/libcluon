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

// clang-format off
#ifdef WIN32
    #include <Winsock2.h> // for htonll
#endif
// clang-format on

#include "cluon/MessageToLCMEncoder.hpp"

#include <cstring>
#include <iostream>

namespace cluon {

std::string MessageToLCMEncoder::encodedData(bool withHash) const noexcept {
    int64_t _hash = hash();
    _hash         = static_cast<int64_t>(htobe64(_hash));
    std::stringstream hashBuffer;
    hashBuffer.write(reinterpret_cast<const char *>(&_hash), sizeof(int64_t));

    const std::string s{(withHash ? hashBuffer.str() : "") + m_buffer.str()};
    return s;
}

////////////////////////////////////////////////////////////////////////////////

void MessageToLCMEncoder::preVisit(uint32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void MessageToLCMEncoder::postVisit() noexcept {}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("boolean");
    calculateHash(0);
    m_buffer.write(reinterpret_cast<char *>(&v), sizeof(bool));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int8_t");
    calculateHash(0);
    m_buffer.write(reinterpret_cast<char *>(&v), sizeof(char));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int8_t");
    calculateHash(0);
    m_buffer.write(reinterpret_cast<char *>(&v), sizeof(int8_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int8_t");
    calculateHash(0);
    m_buffer.write(reinterpret_cast<char *>(&v), sizeof(uint8_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int16_t");
    calculateHash(0);
    int16_t _v = static_cast<int16_t>(htobe16(v));
    m_buffer.write(reinterpret_cast<char *>(&_v), sizeof(int16_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int16_t");
    calculateHash(0);
    int16_t _v = static_cast<int16_t>(htobe16(v));
    m_buffer.write(reinterpret_cast<char *>(&_v), sizeof(int16_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int32_t");
    calculateHash(0);
    int32_t _v = static_cast<int32_t>(htobe32(v));
    m_buffer.write(reinterpret_cast<char *>(&_v), sizeof(int32_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int32_t");
    calculateHash(0);
    int32_t _v = static_cast<int32_t>(htobe32(v));
    m_buffer.write(reinterpret_cast<char *>(&_v), sizeof(int32_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int64_t");
    calculateHash(0);
    int64_t _v = static_cast<int64_t>(htobe64(v));
    m_buffer.write(reinterpret_cast<char *>(&_v), sizeof(int64_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int64_t");
    calculateHash(0);
    int64_t _v = static_cast<int64_t>(htobe64(v));
    m_buffer.write(reinterpret_cast<char *>(&_v), sizeof(int64_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("float");
    calculateHash(0);
    int32_t _v{0};
    std::memmove(&_v, &v, sizeof(int32_t));
    _v = static_cast<int32_t>(htobe32(_v));
    m_buffer.write(reinterpret_cast<char *>(&_v), sizeof(int32_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("double");
    calculateHash(0);
    int64_t _v{0};
    std::memmove(&_v, &v, sizeof(int64_t));
    _v = static_cast<int64_t>(htobe64(_v));
    m_buffer.write(reinterpret_cast<char *>(&_v), sizeof(int64_t));
}

void MessageToLCMEncoder::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("string");
    calculateHash(0);
    const uint32_t length = static_cast<uint32_t>(v.length() + 1);
    int32_t _v            = static_cast<int32_t>(htobe32(length)); // +1 to include the terminating 0.
    m_buffer.write(reinterpret_cast<char *>(&_v), sizeof(int32_t));
    m_buffer.write(v.c_str(), length);
}

////////////////////////////////////////////////////////////////////////////////

int64_t MessageToLCMEncoder::hash() const noexcept {
    // Apply ZigZag encoding for hash.
    const int64_t hash = (m_hash << 1) + ((m_hash >> 63) & 1);
    return hash;
}

void MessageToLCMEncoder::calculateHash(char c) noexcept {
    m_hash = ((m_hash << 8) ^ (m_hash >> 55)) + c;
}

void MessageToLCMEncoder::calculateHash(const std::string &s) noexcept {
    const std::string tmp{(s.length() > 255 ? s.substr(0, 255) : s)};
    const uint8_t length{static_cast<uint8_t>(tmp.length())};
    calculateHash(static_cast<char>(length));
    for (auto c : s) { calculateHash(c); }
}

} // namespace cluon
