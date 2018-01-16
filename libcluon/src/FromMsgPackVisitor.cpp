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

#include "cluon/FromMsgPackVisitor.hpp"

#include <cstring>

namespace cluon {

//void FromMsgPackVisitor::readBytesFromStream(std::istream &in,
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

void FromMsgPackVisitor::decodeFrom(std::istream &in) noexcept {
    (void)in;
//    // Reset internal states as this deserializer could be reused.
//    m_buffer.str("");
//    m_mapOfKeyValues.clear();

//    while (in.good()) {
//        // First stage: Read keyFieldType (encoded as VarInt).
//        uint64_t keyFieldType{0};
//        std::size_t bytesRead{fromVarInt(in, keyFieldType)};

//        if (bytesRead > 0) {
//            // Succeeded to read keyFieldType entry; extract information.
//            const uint32_t fieldId{static_cast<uint32_t>(keyFieldType >> 3)};
//            const ProtoConstants protoType{static_cast<ProtoConstants>(keyFieldType & 0x7)};

//            if (protoType == ProtoConstants::VARINT) {
//                // Directly decode VarInt value.
//                uint64_t value{0};
//                fromVarInt(in, value);
//                ProtoKeyValue pkv{fieldId, value};
//                m_mapOfKeyValues[pkv.key()] = std::move(pkv);
//            } else if (protoType == ProtoConstants::EIGHT_BYTES) {
//                constexpr std::size_t BYTES_TO_READ_FROM_STREAM{sizeof(double)};
//                // Create map entry for Proto key/value here to avoid copying data later.
//                ProtoKeyValue pkv{fieldId, ProtoConstants::EIGHT_BYTES, BYTES_TO_READ_FROM_STREAM};
//                readBytesFromStream(in, BYTES_TO_READ_FROM_STREAM, pkv.rawBuffer());
//                m_mapOfKeyValues[pkv.key()] = std::move(pkv);
//            } else if (protoType == ProtoConstants::LENGTH_DELIMITED) {
//                uint64_t length{0};
//                fromVarInt(in, length);
//                const std::size_t BYTES_TO_READ_FROM_STREAM{static_cast<std::size_t>(length)};
//                // Create map entry for Proto key/value here to avoid copying data later.
//                ProtoKeyValue pkv{fieldId, ProtoConstants::LENGTH_DELIMITED, BYTES_TO_READ_FROM_STREAM};
//                readBytesFromStream(in, BYTES_TO_READ_FROM_STREAM, pkv.rawBuffer());
//                m_mapOfKeyValues[pkv.key()] = std::move(pkv);
//            } else if (protoType == ProtoConstants::FOUR_BYTES) {
//                constexpr std::size_t BYTES_TO_READ_FROM_STREAM{sizeof(float)};
//                // Create map entry for Proto key/value here to avoid copying data later.
//                ProtoKeyValue pkv{fieldId, ProtoConstants::FOUR_BYTES, BYTES_TO_READ_FROM_STREAM};
//                readBytesFromStream(in, BYTES_TO_READ_FROM_STREAM, pkv.rawBuffer());
//                m_mapOfKeyValues[pkv.key()] = std::move(pkv);
//            }
//        }
//    }
}

void FromMsgPackVisitor::preVisit(uint32_t id,
                                       const std::string &shortName,
                                       const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void FromMsgPackVisitor::postVisit() noexcept {}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

} // namespace cluon
