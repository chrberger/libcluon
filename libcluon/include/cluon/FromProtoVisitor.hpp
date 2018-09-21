/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_FROMPROTOVISITOR_HPP
#define CLUON_FROMPROTOVISITOR_HPP

#include "cluon/ProtoConstants.hpp"
#include "cluon/cluon.hpp"
#include "cluon/any/any.hpp"

#include <cstdint>
#include <array>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace cluon {
    class ValueAsHash {
       public:
        inline std::size_t operator()(const uint32_t v) const noexcept {
            return static_cast<std::size_t>(v);
        }
    };

/**
This class decodes a given message from Proto format.
*/
class LIBCLUON_API FromProtoVisitor {
   private:
    FromProtoVisitor(const FromProtoVisitor &) = delete;
    FromProtoVisitor(FromProtoVisitor &&)      = delete;
    FromProtoVisitor &operator=(FromProtoVisitor &&) = delete;

   public:
    FromProtoVisitor()  = default;
    ~FromProtoVisitor() = default;

   public:
    FromProtoVisitor &operator=(const FromProtoVisitor &other) noexcept;

    /**
     * This method decodes a given istream into Proto.
     *
     * @param in istream to decode.
     */
    void decodeFrom(std::istream &in) noexcept;

   public:
    // The following methods are provided to allow an instance of this class to
    // be used as visitor for an instance with the method signature void accept<T>(T&);

    void preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept;
    void postVisit() noexcept;

    void visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept;

    template <typename T>
    void visit(uint32_t &id, std::string &&typeName, std::string &&name, T &v) noexcept {
        (void)typeName;
        (void)name;

        if (m_callToDecodeFromWithDirectVisit) {
            std::stringstream sstr{std::move(std::string(m_stringValue.data(), m_value))};
            cluon::FromProtoVisitor nestedProtoDecoder;
            nestedProtoDecoder.decodeFrom(sstr);
            v.accept(nestedProtoDecoder);
            // TODO: Extend GenericMessage to enable the following call and remove the previous two lines.
//            nestedProtoDecoder.decodeFrom(sstr, v);
        }
        else if (0 < m_mapOfKeyValues.count(id)) {
            try {
                std::stringstream sstr{linb::any_cast<std::string>(m_mapOfKeyValues[id])};
                cluon::FromProtoVisitor nestedProtoDecoder;
                nestedProtoDecoder.decodeFrom(sstr);
                v.accept(nestedProtoDecoder);
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        }
    }

   public:
    /**
     * This method decodes a given istream into corresponding fields of v.
     *
     * @param in istream to decode.
     * @param v Data structure to receive the decoded values.
     */
    template<typename T>
    void decodeFrom(std::istream &in, T &v) noexcept {
        m_callToDecodeFromWithDirectVisit = true;
        while (in.good()) {
            // First stage: Read keyFieldType (encoded as VarInt).
            if (0 < fromVarInt(in, m_keyFieldType)) {
                // Succeeded to read keyFieldType entry; extract information.
                m_protoType = static_cast<ProtoConstants>(m_keyFieldType & 0x7);
                m_fieldId = static_cast<uint32_t>(m_keyFieldType >> 3);
                switch (m_protoType) {
                    case ProtoConstants::VARINT:
                    {
                        // Directly decode VarInt value.
                        fromVarInt(in, m_value);
                        v.accept(m_fieldId, *this);
                    }
                    break;
                    case ProtoConstants::EIGHT_BYTES:
                    {
                        readBytesFromStream(in, sizeof(double), m_doubleValue.buffer.data());
                        m_doubleValue.uint64Value = le64toh(m_doubleValue.uint64Value);
                        v.accept(m_fieldId, *this);
                    }
                    break;
                    case ProtoConstants::FOUR_BYTES:
                    {
                        readBytesFromStream(in, sizeof(float), m_floatValue.buffer.data());
                        m_floatValue.uint32Value = le32toh(m_floatValue.uint32Value);
                        v.accept(m_fieldId, *this);
                    }
                    break;
                    case ProtoConstants::LENGTH_DELIMITED:
                    {
                        fromVarInt(in, m_value);
                        const std::size_t BYTES_TO_READ_FROM_STREAM{static_cast<std::size_t>(m_value)};
                        if (m_stringValue.capacity() < BYTES_TO_READ_FROM_STREAM) {
                            m_stringValue.reserve(BYTES_TO_READ_FROM_STREAM);
                        }
                        readBytesFromStream(in, BYTES_TO_READ_FROM_STREAM, m_stringValue.data());
                        v.accept(m_fieldId, *this);
                    }
                    break;
                }
            }
        }
        m_callToDecodeFromWithDirectVisit = false;
    }

   private:
    int8_t fromZigZag8(uint8_t v) noexcept;
    int16_t fromZigZag16(uint16_t v) noexcept;
    int32_t fromZigZag32(uint32_t v) noexcept;
    int64_t fromZigZag64(uint64_t v) noexcept;

    std::size_t fromVarInt(std::istream &in, uint64_t &value) noexcept;

    void readBytesFromStream(std::istream &in, std::size_t bytesToReadFromStream, char *buffer) noexcept;

   private:
    // This Boolean flag indicates whether we consecutively decode from istream
    // and inject the decoded values directly into the receiving data structure.
    bool m_callToDecodeFromWithDirectVisit{false};
    std::unordered_map<uint32_t, linb::any, ValueAsHash> m_mapOfKeyValues{};

   private:
    // Fields necessary to decode from an istream.
    uint64_t m_value{0};

    // Union buffer for double values.
    union DoubleValue {
        std::array<char, sizeof(double)> buffer;
        uint64_t uint64Value;
        double doubleValue{0};
    } m_doubleValue;

    // Union buffer for float values.
    union FloatValue {
        std::array<char, sizeof(float)> buffer;
        uint32_t uint32Value;
        float floatValue{0};
    } m_floatValue;

    // Buffer for strings.
    std::vector<char> m_stringValue;

    uint64_t m_keyFieldType{0};
    ProtoConstants m_protoType{ProtoConstants::VARINT};
    uint32_t m_fieldId{0};
};
} // namespace cluon

#endif
