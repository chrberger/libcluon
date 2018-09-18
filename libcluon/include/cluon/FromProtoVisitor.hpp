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
    void visit(uint32_t &id, std::string &&typeName, std::string &&name, T &value) noexcept {
        (void)typeName;
        (void)name;

        if (0 < m_mapOfKeyValues.count(id)) {
            try {
                std::stringstream sstr{linb::any_cast<std::string>(m_mapOfKeyValues[id])};
                cluon::FromProtoVisitor nestedProtoDecoder;
                nestedProtoDecoder.decodeFrom(sstr);

                value.accept(nestedProtoDecoder);
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        }
    }

   private:
    int8_t fromZigZag8(uint8_t v) noexcept;
    int16_t fromZigZag16(uint16_t v) noexcept;
    int32_t fromZigZag32(uint32_t v) noexcept;
    int64_t fromZigZag64(uint64_t v) noexcept;

    std::size_t fromVarInt(std::istream &in, uint64_t &value) noexcept;

    void readBytesFromStream(std::istream &in, std::size_t bytesToReadFromStream, char *buffer) noexcept;

   private:
    std::unordered_map<uint32_t, linb::any, ValueAsHash> m_mapOfKeyValues{};

    bool m_needToResetPreallocatedEntries{false};
    const constexpr static uint8_t MAX_PREALLOCATED_ENTRIES{20};
    std::unordered_map<uint32_t, uint64_t, ValueAsHash> preallocatedMapOfKeyVarInts = {
        {0, 0},
        {1, 0},
        {2, 0},
        {3, 0},
        {4, 0},
        {5, 0},
        {6, 0},
        {7, 0},
        {8, 0},
        {9, 0},
        {10, 0},
        {11, 0},
        {12, 0},
        {13, 0},
        {14, 0},
        {15, 0},
        {16, 0},
        {17, 0},
        {18, 0},
        {19, 0}
    };

    std::unordered_map<uint32_t, float, ValueAsHash> preallocatedMapOfKeyFloats = {
        {0, 0.f},
        {1, 0.f},
        {2, 0.f},
        {3, 0.f},
        {4, 0.f},
        {5, 0.f},
        {6, 0.f},
        {7, 0.f},
        {8, 0.f},
        {9, 0.f},
        {10, 0.f},
        {11, 0.f},
        {12, 0.f},
        {13, 0.f},
        {14, 0.f},
        {15, 0.f},
        {16, 0.f},
        {17, 0.f},
        {18, 0.f},
        {19, 0.f}
    };

    std::unordered_map<uint32_t, double, ValueAsHash> preallocatedMapOfKeyDoubles = {
        {0, 0.0},
        {1, 0.0},
        {2, 0.0},
        {3, 0.0},
        {4, 0.0},
        {5, 0.0},
        {6, 0.0},
        {7, 0.0},
        {8, 0.0},
        {9, 0.0},
        {10, 0.0},
        {11, 0.0},
        {12, 0.0},
        {13, 0.0},
        {14, 0.0},
        {15, 0.0},
        {16, 0.0},
        {17, 0.0},
        {18, 0.0},
        {19, 0.0}
    };

    std::unordered_map<uint32_t, std::string, ValueAsHash> preallocatedMapOfKeyStrings = {
        {0, ""},
        {1, ""},
        {2, ""},
        {3, ""},
        {4, ""},
        {5, ""},
        {6, ""},
        {7, ""},
        {8, ""},
        {9, ""},
        {10, ""},
        {11, ""},
        {12, ""},
        {13, ""},
        {14, ""},
        {15, ""},
        {16, ""},
        {17, ""},
        {18, ""},
        {19, ""}
    };

   private:
    // VARINT values.
    uint64_t m_value{0};

    // Buffer for double values.
    union DoubleValue {
        std::array<char, sizeof(double)> buffer;
        uint64_t uint64Value;
        double doubleValue{0};
    } m_doubleValue;

    // Buffer for float values.
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
