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

#include "cluon/MessageFromProtoDecoder.hpp"
#include <cstring>
#include <iostream>

namespace cluon {

void MessageFromProtoDecoder::readBytesFromStream(std::istream &in,
                                                  const std::size_t &bytesToReadFromStream,
                                                  std::vector<char> &buffer) noexcept {
    constexpr std::size_t CHUNK_SIZE{1024};
    std::size_t bytesToRead{bytesToReadFromStream};
    std::streamsize bufferPosition{0};

    // Ensure buffer has enough space to hold the bytes.
    buffer.reserve(bytesToRead);

    while ((0 < bytesToRead) && in.good()) {
        // clang-format off
        in.read(&buffer[static_cast<std::size_t>(bufferPosition)], /* Flawfinder: ignore */ /* Cf. buffer.reserve(...) above.  */
                (bytesToRead > CHUNK_SIZE) ? CHUNK_SIZE : static_cast<std::streamsize>(bytesToRead));
        // clang-format on
        const std::streamsize EXTRACTED_BYTES{in.gcount()};
        bufferPosition += EXTRACTED_BYTES;
        bytesToRead -= static_cast<std::size_t>(EXTRACTED_BYTES);
    }
}

void MessageFromProtoDecoder::decodeFrom(std::istream &in) noexcept {
    // Reset internal states as this deserializer could be reused.
    m_buffer.str("");
    m_mapOfKeyValues.clear();

    while (in.good()) {
        // First stage: Read keyFieldType (encoded as VarInt).
        uint64_t keyFieldType{0};
        std::size_t bytesRead{fromVarInt(in, keyFieldType)};

        if (bytesRead > 0) {
            // Succeeded to read keyFieldType entry; extract information.
            const uint32_t fieldId{static_cast<uint32_t>(keyFieldType >> 3)};
            const ProtoConstants protoType{static_cast<ProtoConstants>(keyFieldType & 0x7)};

            if (protoType == ProtoConstants::VARINT) {
                // Directly decode VarInt value.
                uint64_t value{0};
                fromVarInt(in, value);
                ProtoKeyValue pkv{fieldId, value};
                m_mapOfKeyValues[pkv.key()] = std::move(pkv);
            } else if (protoType == ProtoConstants::EIGHT_BYTES) {
                constexpr std::size_t BYTES_TO_READ_FROM_STREAM{sizeof(double)};
                // Create map entry for Proto key/value here to avoid copying data later.
                ProtoKeyValue pkv{fieldId, ProtoConstants::EIGHT_BYTES, BYTES_TO_READ_FROM_STREAM};
                readBytesFromStream(in, BYTES_TO_READ_FROM_STREAM, pkv.rawBuffer());
                m_mapOfKeyValues[pkv.key()] = std::move(pkv);
            } else if (protoType == ProtoConstants::LENGTH_DELIMITED) {
                uint64_t length{0};
                fromVarInt(in, length);
                const std::size_t BYTES_TO_READ_FROM_STREAM{static_cast<std::size_t>(length)};
                // Create map entry for Proto key/value here to avoid copying data later.
                ProtoKeyValue pkv{fieldId, ProtoConstants::LENGTH_DELIMITED, BYTES_TO_READ_FROM_STREAM};
                readBytesFromStream(in, BYTES_TO_READ_FROM_STREAM, pkv.rawBuffer());
                m_mapOfKeyValues[pkv.key()] = std::move(pkv);
            } else if (protoType == ProtoConstants::FOUR_BYTES) {
                constexpr std::size_t BYTES_TO_READ_FROM_STREAM{sizeof(float)};
                // Create map entry for Proto key/value here to avoid copying data later.
                ProtoKeyValue pkv{fieldId, ProtoConstants::FOUR_BYTES, BYTES_TO_READ_FROM_STREAM};
                readBytesFromStream(in, BYTES_TO_READ_FROM_STREAM, pkv.rawBuffer());
                m_mapOfKeyValues[pkv.key()] = std::move(pkv);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

MessageFromProtoDecoder::ProtoKeyValue::ProtoKeyValue() noexcept
    : m_key{0}
    , m_type{ProtoConstants::VARINT}
    , m_length{0}
    , m_value{}
    , m_varIntValue{0} {}

MessageFromProtoDecoder::ProtoKeyValue::ProtoKeyValue(uint32_t key, ProtoConstants type, uint64_t length) noexcept
    : m_key{key}
    , m_type{type}
    , m_length{length}
    , m_value(length)
    , m_varIntValue{0} {}

MessageFromProtoDecoder::ProtoKeyValue::ProtoKeyValue(uint32_t key, uint64_t value) noexcept
    : m_key{key}
    , m_type{ProtoConstants::VARINT}
    , m_length{0}
    , m_value{}
    , m_varIntValue{value} {}

uint32_t MessageFromProtoDecoder::ProtoKeyValue::key() const noexcept {
    return m_key;
}

ProtoConstants MessageFromProtoDecoder::ProtoKeyValue::type() const noexcept {
    return m_type;
}

uint64_t MessageFromProtoDecoder::ProtoKeyValue::length() const noexcept {
    return m_length;
}

uint64_t MessageFromProtoDecoder::ProtoKeyValue::valueAsVarInt() const noexcept {
    uint64_t retVal{0};
    if (type() == ProtoConstants::VARINT) {
        retVal = m_varIntValue;
    }
    return retVal;
}

float MessageFromProtoDecoder::ProtoKeyValue::valueAsFloat() const noexcept {
    float retVal{0};
    if (!m_value.empty() && (length() == sizeof(float)) && (m_value.size() == sizeof(float))
        && (type() == ProtoConstants::FOUR_BYTES)) {
        std::memmove(&retVal, &m_value[0], sizeof(float));
    }
    return retVal;
}

double MessageFromProtoDecoder::ProtoKeyValue::valueAsDouble() const noexcept {
    double retVal{0};
    if (!m_value.empty() && (length() == sizeof(double)) && (m_value.size() == sizeof(double))
        && (type() == ProtoConstants::EIGHT_BYTES)) {
        std::memmove(&retVal, &m_value[0], sizeof(double));
    }
    return retVal;
}

std::string MessageFromProtoDecoder::ProtoKeyValue::valueAsString() const noexcept {
    std::string retVal;
    if (!m_value.empty() && (length() > 0) && (type() == ProtoConstants::LENGTH_DELIMITED)) {
        // Create string from buffer.
        retVal = std::string(m_value.data(), m_length);
    }
    return retVal;
}

std::vector<char> &MessageFromProtoDecoder::ProtoKeyValue::rawBuffer() noexcept {
    return m_value;
}

////////////////////////////////////////////////////////////////////////////////

MessageFromProtoDecoder &MessageFromProtoDecoder::operator=(const MessageFromProtoDecoder &other) noexcept {
    m_buffer.str(other.m_buffer.str());
    m_mapOfKeyValues = other.m_mapOfKeyValues;

    return *this;
}

////////////////////////////////////////////////////////////////////////////////

void MessageFromProtoDecoder::visit(uint32_t id, bool &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        v = (0 != m_mapOfKeyValues[id].valueAsVarInt());
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, char &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        uint64_t _v = m_mapOfKeyValues[id].valueAsVarInt();
        v           = static_cast<char>(_v);
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, int8_t &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        uint64_t _v = m_mapOfKeyValues[id].valueAsVarInt();
        v           = static_cast<int8_t>(fromZigZag8(static_cast<uint8_t>(_v)));
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, uint8_t &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        uint64_t _v = m_mapOfKeyValues[id].valueAsVarInt();
        v           = static_cast<uint8_t>(_v);
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, int16_t &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        uint64_t _v = m_mapOfKeyValues[id].valueAsVarInt();
        v           = static_cast<int16_t>(fromZigZag16(static_cast<uint16_t>(_v)));
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, uint16_t &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        uint64_t _v = m_mapOfKeyValues[id].valueAsVarInt();
        v           = static_cast<uint16_t>(_v);
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, int32_t &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        uint64_t _v = m_mapOfKeyValues[id].valueAsVarInt();
        v           = static_cast<int32_t>(fromZigZag32(static_cast<uint32_t>(_v)));
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, uint32_t &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        uint64_t _v = m_mapOfKeyValues[id].valueAsVarInt();
        v           = static_cast<uint32_t>(_v);
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, int64_t &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        uint64_t _v = m_mapOfKeyValues[id].valueAsVarInt();
        v           = static_cast<int64_t>(fromZigZag64(_v));
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, uint64_t &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        v = m_mapOfKeyValues[id].valueAsVarInt();
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, float &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        v = m_mapOfKeyValues[id].valueAsFloat();
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, double &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        v = m_mapOfKeyValues[id].valueAsDouble();
    }
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &v) noexcept {
    if (m_mapOfKeyValues.count(id) > 0) {
        v = m_mapOfKeyValues[id].valueAsString();
    }
}

////////////////////////////////////////////////////////////////////////////////

void MessageFromProtoDecoder::preVisit(uint32_t id,
                                       const std::string &shortName,
                                       const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void MessageFromProtoDecoder::postVisit() noexcept {}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

void MessageFromProtoDecoder::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    (void)name;
    visit(id, v);
}

////////////////////////////////////////////////////////////////////////////////

int8_t MessageFromProtoDecoder::fromZigZag8(uint8_t v) noexcept {
    return static_cast<int8_t>((v >> 1) ^ -(v & 1));
}

int16_t MessageFromProtoDecoder::fromZigZag16(uint16_t v) noexcept {
    return static_cast<int16_t>((v >> 1) ^ -(v & 1));
}

int32_t MessageFromProtoDecoder::fromZigZag32(uint32_t v) noexcept {
    return static_cast<int32_t>((v >> 1) ^ -(v & 1));
}

int64_t MessageFromProtoDecoder::fromZigZag64(uint64_t v) noexcept {
    return static_cast<int64_t>((v >> 1) ^ -(v & 1));
}

std::size_t MessageFromProtoDecoder::fromVarInt(std::istream &in, uint64_t &value) noexcept {
    value = 0;

    constexpr uint64_t MASK  = 0x7f;
    constexpr uint64_t SHIFT = 0x7;
    constexpr uint64_t MSB   = 0x80;

    std::size_t size = 0;
    while (in.good()) {
        const auto C     = in.get();
        const uint64_t B = static_cast<uint64_t>(C) & MASK;
        value |= B << (SHIFT * size++);
        if (!(static_cast<uint64_t>(C) & MSB)) { // NOLINT
            break;
        }
    }

    // VarInt is little endian.
    value = le64toh(value);
    return size;
}
} // namespace cluon
