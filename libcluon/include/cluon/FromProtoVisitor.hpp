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

#include <cstdint>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace cluon {
/**
This class decodes a given message from Proto format.
*/
class LIBCLUON_API FromProtoVisitor {
    /**
     * This class represents an entry in a Proto payload stream.
     */
    class ProtoKeyValue {
       private:
        ProtoKeyValue(ProtoKeyValue &&) = delete;
        ProtoKeyValue &operator=(const ProtoKeyValue &) = delete;

       public:
        ProtoKeyValue() noexcept;
        ProtoKeyValue(const ProtoKeyValue &) = default; // LCOV_EXCL_LINE
        ProtoKeyValue &operator=(ProtoKeyValue &&) = default;
        ~ProtoKeyValue()                           = default;

        /**
         * Constructor to pre-allocate the vector<char> for length-delimited types.
         *
         * @param key Proto key.
         * @param type Proto type.
         * @param length Length of the contained value.
         */
        ProtoKeyValue(uint32_t key, ProtoConstants type, uint64_t length) noexcept;

        /**
         * Constructor for cases when a VARINT value is encoded.
         *
         * @param key Proto key.
         * @param value Actual VarInt value.
         */
        ProtoKeyValue(uint32_t key, uint64_t value) noexcept;

        uint32_t key() const noexcept;
        ProtoConstants type() const noexcept;
        uint64_t length() const noexcept;

        uint64_t valueAsVarInt() const noexcept;
        float valueAsFloat() const noexcept;
        double valueAsDouble() const noexcept;
        std::string valueAsString() const noexcept;

        /**
         * @return Raw value as reference.
         */
        std::vector<char> &rawBuffer() noexcept;

       private:
        uint32_t m_key{0};
        ProtoConstants m_type{ProtoConstants::VARINT};
        uint64_t m_length{0};
        std::vector<char> m_value{};
        uint64_t m_varIntValue{0};
    };

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
            const std::string s{m_mapOfKeyValues[id].valueAsString()};

            std::stringstream sstr{s};
            cluon::FromProtoVisitor nestedProtoDecoder;
            nestedProtoDecoder.decodeFrom(sstr);

            value.accept(nestedProtoDecoder);
        }
    }

   private:
    int8_t fromZigZag8(uint8_t v) noexcept;
    int16_t fromZigZag16(uint16_t v) noexcept;
    int32_t fromZigZag32(uint32_t v) noexcept;
    int64_t fromZigZag64(uint64_t v) noexcept;

    std::size_t fromVarInt(std::istream &in, uint64_t &value) noexcept;

    void readBytesFromStream(std::istream &in, std::size_t bytesToReadFromStream, std::vector<char> &buffer) noexcept;

   private:
    std::stringstream m_buffer{""};
    std::map<uint32_t, ProtoKeyValue> m_mapOfKeyValues{};
};
} // namespace cluon

#endif
