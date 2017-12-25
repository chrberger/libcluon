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
    #include <Winsock2.h> // for ntohll
#endif
// clang-format on

#include "cluon/MessageFromLCMDecoder.hpp"

#include <cstring>
#include <iostream>
#include <vector>

namespace cluon {

void MessageFromLCMDecoder::decodeFrom(std::istream &in, bool hasHash) noexcept {
    // Reset internal states as this deserializer could be reused.
    m_buffer.clear();
    m_buffer.str("");

    m_expectedHash = 0;
    if (hasHash) {
        in.read(reinterpret_cast<char *>(&m_expectedHash), sizeof(int64_t));
        m_expectedHash = static_cast<int64_t>(be64toh(m_expectedHash));
    }

    m_buffer << in.rdbuf();
}

////////////////////////////////////////////////////////////////////////////////

void MessageFromLCMDecoder::preVisit(uint32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;

    // Reset m_buffer read pointer to beginning.
    m_buffer.clear();
    m_buffer.seekg(0);
    m_calculatedHash = 0x12345678;
}

void MessageFromLCMDecoder::postVisit() noexcept {
    if ((0 != m_expectedHash) && (m_expectedHash != hash())) {
        std::cerr << "[cluon::MessageFromLCMDecoder] Hash mismatch - decoding has failed" << std::endl; // NOLINT
    }
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("boolean");
    calculateHash(0);
    m_buffer.read(reinterpret_cast<char *>(&v), sizeof(bool));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int8_t");
    calculateHash(0);
    m_buffer.read(reinterpret_cast<char *>(&v), sizeof(char));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int8_t");
    calculateHash(0);
    m_buffer.read(reinterpret_cast<char *>(&v), sizeof(int8_t));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int8_t");
    calculateHash(0);
    m_buffer.read(reinterpret_cast<char *>(&v), sizeof(int8_t));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int16_t");
    calculateHash(0);
    int16_t _v{0};
    m_buffer.read(reinterpret_cast<char *>(&_v), sizeof(int16_t));
    v = static_cast<int16_t>(be16toh(_v));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int16_t");
    calculateHash(0);
    int16_t _v{0};
    m_buffer.read(reinterpret_cast<char *>(&_v), sizeof(int16_t));
    v = be16toh(_v);
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int32_t");
    calculateHash(0);
    int32_t _v{0};
    m_buffer.read(reinterpret_cast<char *>(&_v), sizeof(int32_t));
    v = static_cast<int32_t>(be32toh(_v));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int32_t");
    calculateHash(0);
    int32_t _v{0};
    m_buffer.read(reinterpret_cast<char *>(&_v), sizeof(int32_t));
    v = be32toh(_v);
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int64_t");
    calculateHash(0);
    int64_t _v{0};
    m_buffer.read(reinterpret_cast<char *>(&_v), sizeof(int64_t));
    v = static_cast<int64_t>(be64toh(_v));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int64_t");
    calculateHash(0);
    int64_t _v{0};
    m_buffer.read(reinterpret_cast<char *>(&_v), sizeof(int64_t));
    v = be64toh(_v);
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("float");
    calculateHash(0);
    int32_t _v{0};
    m_buffer.read(reinterpret_cast<char *>(&_v), sizeof(int32_t));
    _v = static_cast<int32_t>(be32toh(_v));
    std::memmove(&v, &_v, sizeof(int32_t));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("double");
    calculateHash(0);
    int64_t _v{0};
    m_buffer.read(reinterpret_cast<char *>(&_v), sizeof(int64_t));
    _v = static_cast<int64_t>(be64toh(_v));
    std::memmove(&v, &_v, sizeof(int64_t));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
    calculateHash(name);
    calculateHash("string");
    calculateHash(0);

    int32_t length{0};
    m_buffer.read(reinterpret_cast<char *>(&length), sizeof(int32_t));
    length = static_cast<int32_t>(be32toh(length));

    std::vector<char> buffer;
    buffer.reserve(static_cast<uint32_t>(length));
    // Read data but skip trailing \0.
    for (uint32_t i = 0; i < static_cast<uint32_t>(length-1); i++) {
        char c;
        m_buffer.get(c);
        buffer.push_back(c);
    }

    const std::string s(buffer.begin(), buffer.end());
    v = s;
}

////////////////////////////////////////////////////////////////////////////////

int64_t MessageFromLCMDecoder::hash() const noexcept {
    // Apply ZigZag encoding for hash.
    const int64_t hash = (m_calculatedHash << 1) + ((m_calculatedHash >> 63) & 1);
    return hash;
}

void MessageFromLCMDecoder::calculateHash(char c) noexcept {
    m_calculatedHash = ((m_calculatedHash << 8) ^ (m_calculatedHash >> 55)) + c;
}

void MessageFromLCMDecoder::calculateHash(const std::string &s) noexcept {
    const std::string tmp{(s.length() > 255 ? s.substr(0, 255) : s)};
    const uint8_t length{static_cast<uint8_t>(tmp.length())};
    calculateHash(static_cast<char>(length));
    for (auto c : s) { calculateHash(c); }
}

} // namespace cluon
