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

#include "cluon/MessageFromLCMDecoder.hpp"
#include <cstring>
#include <iostream>

namespace cluon {

//void MessageFromLCMDecoder::readBytesFromStream(std::istream &in,
//                                                  std::size_t bytesToReadFromStream,
//                                                  std::vector<char> &buffer) noexcept {
//    constexpr std::size_t CHUNK_SIZE{1024};
//    std::streamsize bufferPosition{0};

//    // Ensure buffer has enough space to hold the bytes.
//    buffer.reserve(bytesToReadFromStream);

//    while ((0 < bytesToReadFromStream) && in.good()) {
//        // clang-format off
//        in.read(&buffer[static_cast<std::size_t>(bufferPosition)], /* Flawfinder: ignore */ /* Cf. buffer.reserve(...) above.  */
//                (bytesToReadFromStream > CHUNK_SIZE) ? CHUNK_SIZE : static_cast<std::streamsize>(bytesToReadFromStream));
//        // clang-format on
//        const std::streamsize EXTRACTED_BYTES{in.gcount()};
//        bufferPosition += EXTRACTED_BYTES;
//        bytesToReadFromStream -= static_cast<std::size_t>(EXTRACTED_BYTES);
//    }
//}

void MessageFromLCMDecoder::decodeFrom(std::istream &in) noexcept {
    // Reset internal states as this deserializer could be reused.
    m_buffer.str("");

    in.read(reinterpret_cast<char*>(&m_expectedHash), sizeof(int64_t));
    m_expectedHash = static_cast<int64_t>(be64toh(m_expectedHash));

    m_buffer << in.rdbuf();

//    // Transfer the remaining data into our internal buffer.
//    while (in.good()) {
//        c = in.get();
//        m_buffer.put(c);
//    }
}

////////////////////////////////////////////////////////////////////////////////

void MessageFromLCMDecoder::preVisit(uint32_t id,
                                       const std::string &shortName,
                                       const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;

    m_buffer.clear();
    m_buffer.seekg(0);
    m_calculatedHash = 0x12345678;

    // TODO: Reset m_buffer read pointer to beginning.
}

void MessageFromLCMDecoder::postVisit() noexcept {
    if (m_expectedHash != hash()) {
        std::cerr << "[cluon::MessageFromLCMDecoder] Hash mismatch - decoding has failed" << std::endl;
    }
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("boolean");
    calculateHash(0);
    m_buffer.read(reinterpret_cast<char*>(&v), sizeof(bool));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)id;
    (void)typeName;
    calculateHash(name);
    calculateHash("int8_t");
    calculateHash(0);
    m_buffer.read(reinterpret_cast<char*>(&v), sizeof(char));
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

void MessageFromLCMDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;
    (void)name;
    (void)v;
}

////////////////////////////////////////////////////////////////////////////////

int64_t MessageFromLCMDecoder::hash() const noexcept {
    // Apply ZigZag encoding for hash.
    const int64_t hash = (m_calculatedHash << 1) + ( (m_calculatedHash >> 63) & 1 );
    return hash;
}

void MessageFromLCMDecoder::calculateHash(char c) noexcept {
    m_calculatedHash = ((m_calculatedHash<<8) ^ (m_calculatedHash>>55)) + c;
}

void MessageFromLCMDecoder::calculateHash(const std::string &s) noexcept {
    const std::string tmp{(s.length() > 255 ? s.substr(0, 255) : s)};
    const uint8_t length{static_cast<uint8_t>(tmp.length())};
    calculateHash(static_cast<char>(length));
    for (auto c : s) {
        calculateHash(c);
    }
}

} // namespace cluon
