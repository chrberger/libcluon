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

#ifndef FROMMSGPACKVISITOR_HPP
#define FROMMSGPACKVISITOR_HPP

#include "cluon/MsgPackConstants.hpp"
#include "cluon/cluon.hpp"
#include "cluon/any/any.hpp"

#include <cstdint>
#include <istream>
#include <map>
#include <string>

namespace cluon {
/**
This class decodes a given message from MsgPack format.
*/
class LIBCLUON_API FromMsgPackVisitor {
    /**
     * This class represents a key/value in a MsgPack payload stream of key/values.
     */
    class MsgPackKeyValue {
       private:
        MsgPackKeyValue &operator=(MsgPackKeyValue &&) = delete;
        MsgPackKeyValue(const MsgPackKeyValue &) = delete;
 
       public:
        MsgPackKeyValue() = default;
        MsgPackKeyValue(MsgPackKeyValue &&) = default;
        MsgPackKeyValue &operator=(const MsgPackKeyValue &) = default;
       ~MsgPackKeyValue() = default;

       public:
        std::string m_key{""};
        MsgPackConstants m_formatFamily{MsgPackConstants::BOOL_FORMAT};
        linb::any m_value;
    };

   private:
    FromMsgPackVisitor(const FromMsgPackVisitor &) = delete;
    FromMsgPackVisitor(FromMsgPackVisitor &&)      = delete;
    FromMsgPackVisitor &operator=(FromMsgPackVisitor &&) = delete;
    FromMsgPackVisitor &operator=(const FromMsgPackVisitor &other) = delete;

   public:
    FromMsgPackVisitor()  = default;
    ~FromMsgPackVisitor() = default;

    /**
     * This method decodes a given istream into Proto.
     *
     * @param in istream to decode.
     */
    void decodeFrom(std::istream &in) noexcept;

   public:
    // The following methods are provided to allow an instance of this class to
    // be used as visitor for an instance with the method signature void accept<T>(T&);

    void preVisit(uint32_t id, const std::string &shortName, const std::string &longName) noexcept;
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
        (void)id;
        (void)typeName;

        (void)name;
        cluon::FromMsgPackVisitor nestedMsgPackDecoder;
//        nestedMsgPackDecoder.decodeFrom(m_buffer);
        value.accept(nestedMsgPackDecoder);
    }

   private:
    MsgPackConstants getFormatFamily(uint8_t T) noexcept;
    std::map<std::string, FromMsgPackVisitor::MsgPackKeyValue> readKeyValues(std::istream &in) noexcept;
    uint64_t readUint(std::istream &in) noexcept;
    int64_t readInt(std::istream &in) noexcept;
    std::string readString(std::istream &in) noexcept;

   private:
    std::map<std::string, FromMsgPackVisitor::MsgPackKeyValue> m_keyValues{};
};
} // namespace cluon

#endif
