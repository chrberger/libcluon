/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/FromProtoVisitor.hpp"

#include <cstddef>
#include <cstring>
#include <array>
#include <utility>
#include <vector>

namespace cluon {

void FromProtoVisitor::readBytesFromStream(std::istream &in, std::size_t bytesToReadFromStream, char *buffer) noexcept {
    if (nullptr != buffer) {
        const constexpr std::size_t CHUNK_SIZE{1024};
        std::streamsize bufferPosition{0};

        while ((0 < bytesToReadFromStream) && in.good()) {
            // clang-format off
            in.read(&buffer[static_cast<std::size_t>(bufferPosition)], /* Flawfinder: ignore */ /* Cf. buffer.reserve(...) above.  */
                    (bytesToReadFromStream > CHUNK_SIZE) ? CHUNK_SIZE : static_cast<std::streamsize>(bytesToReadFromStream));
            // clang-format on
            const std::streamsize EXTRACTED_BYTES{in.gcount()};
            bufferPosition += EXTRACTED_BYTES;
            bytesToReadFromStream -= static_cast<std::size_t>(EXTRACTED_BYTES);
        }
    }
}

void FromProtoVisitor::decodeFrom(std::istream &in) noexcept {
    // Reset internal states as this deserializer could be reused.
    m_mapOfKeyValues.clear();

    // VARINT values.
    uint64_t value{0};

    // Buffer for double values.
    union DoubleValue {
        std::array<char, sizeof(double)> buffer;
        uint64_t uint64Value;
        double doubleValue{0};
    } doubleValue;

    // Buffer for float values.
    union FloatValue {
        std::array<char, sizeof(float)> buffer;
        uint32_t uint32Value;
        float floatValue{0};
    } floatValue;

    // Buffer for strings.
    std::vector<char> buffer;

    uint64_t keyFieldType{0};
    ProtoConstants protoType{ProtoConstants::VARINT};
    uint32_t fieldId{0};
    while (in.good()) {
        // First stage: Read keyFieldType (encoded as VarInt).
        if (0 < fromVarInt(in, keyFieldType)) {
            // Succeeded to read keyFieldType entry; extract information.
            protoType = static_cast<ProtoConstants>(keyFieldType & 0x7);
            fieldId = static_cast<uint32_t>(keyFieldType >> 3);
            switch (protoType) {
                case ProtoConstants::VARINT:
                {
                    // Directly decode VarInt value.
                    fromVarInt(in, value);
                    m_mapOfKeyValues.emplace(fieldId, linb::any(value));
                }
                break;
                case ProtoConstants::EIGHT_BYTES:
                {
                    readBytesFromStream(in, sizeof(double), doubleValue.buffer.data());
                    doubleValue.uint64Value = le64toh(doubleValue.uint64Value);
                    m_mapOfKeyValues.emplace(fieldId, linb::any(doubleValue.doubleValue));
                }
                break;
                case ProtoConstants::FOUR_BYTES:
                {
                    readBytesFromStream(in, sizeof(float), floatValue.buffer.data());
                    floatValue.uint32Value = le32toh(floatValue.uint32Value);
                    m_mapOfKeyValues.emplace(fieldId, linb::any(floatValue.floatValue));
                }
                break;
                case ProtoConstants::LENGTH_DELIMITED:
                {
                    fromVarInt(in, value);
                    const std::size_t BYTES_TO_READ_FROM_STREAM{static_cast<std::size_t>(value)};
                    if (buffer.capacity() < BYTES_TO_READ_FROM_STREAM) {
                        buffer.reserve(BYTES_TO_READ_FROM_STREAM);
                    }
                    readBytesFromStream(in, BYTES_TO_READ_FROM_STREAM, buffer.data());
                    m_mapOfKeyValues.emplace(fieldId, linb::any(std::string(buffer.data(), value)));
                }
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

FromProtoVisitor &FromProtoVisitor::operator=(const FromProtoVisitor &other) noexcept {
    m_mapOfKeyValues = other.m_mapOfKeyValues;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

void FromProtoVisitor::preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void FromProtoVisitor::postVisit() noexcept {}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            v = (0 != linb::any_cast<uint64_t>(m_mapOfKeyValues[id]));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            uint64_t _v = linb::any_cast<uint64_t>(m_mapOfKeyValues[id]);
            v           = static_cast<char>(_v);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            uint64_t _v = linb::any_cast<uint64_t>(m_mapOfKeyValues[id]);
            v           = static_cast<int8_t>(fromZigZag8(static_cast<uint8_t>(_v)));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            uint64_t _v = linb::any_cast<uint64_t>(m_mapOfKeyValues[id]);
            v           = static_cast<uint8_t>(_v);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            uint64_t _v = linb::any_cast<uint64_t>(m_mapOfKeyValues[id]);
            v           = static_cast<int16_t>(fromZigZag16(static_cast<uint16_t>(_v)));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            uint64_t _v = linb::any_cast<uint64_t>(m_mapOfKeyValues[id]);
            v           = static_cast<uint16_t>(_v);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            uint64_t _v = linb::any_cast<uint64_t>(m_mapOfKeyValues[id]);
            v           = static_cast<int32_t>(fromZigZag32(static_cast<uint32_t>(_v)));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            uint64_t _v = linb::any_cast<uint64_t>(m_mapOfKeyValues[id]);
            v           = static_cast<uint32_t>(_v);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            uint64_t _v = linb::any_cast<uint64_t>(m_mapOfKeyValues[id]);
            v           = static_cast<int64_t>(fromZigZag64(_v));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            v = linb::any_cast<uint64_t>(m_mapOfKeyValues[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            v = linb::any_cast<float>(m_mapOfKeyValues[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            v = linb::any_cast<double>(m_mapOfKeyValues[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromProtoVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    (void)name;
    if (m_mapOfKeyValues.count(id) > 0) {
        try {
            v = linb::any_cast<std::string>(m_mapOfKeyValues[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int8_t FromProtoVisitor::fromZigZag8(uint8_t v) noexcept {
    return static_cast<int8_t>((v >> 1) ^ -(v & 1));
}

int16_t FromProtoVisitor::fromZigZag16(uint16_t v) noexcept {
    return static_cast<int16_t>((v >> 1) ^ -(v & 1));
}

int32_t FromProtoVisitor::fromZigZag32(uint32_t v) noexcept {
    return static_cast<int32_t>((v >> 1) ^ -(v & 1));
}

int64_t FromProtoVisitor::fromZigZag64(uint64_t v) noexcept {
    return static_cast<int64_t>((v >> 1) ^ -(v & 1));
}

std::size_t FromProtoVisitor::fromVarInt(std::istream &in, uint64_t &value) noexcept {
    value = 0;

    constexpr uint64_t MASK  = 0x7f;
    constexpr uint64_t SHIFT = 0x7;
    constexpr uint64_t MSB   = 0x80;

    std::size_t size = 0;
    uint64_t C{0};
    while (in.good()) {
        auto x = in.get();
        C = static_cast<uint64_t>(x);
        value |= (C & MASK) << (SHIFT * size++);
        if (!(C & MSB)) { // NOLINT
            break;
        }
    }

    return size;
}
} // namespace cluon
