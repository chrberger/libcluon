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

#ifndef GENERICMESSAGE_HPP
#define GENERICMESSAGE_HPP

#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"
#include "cluon/MessageFromProtoDecoder.hpp"
#include "cluon/MetaMessage.hpp"

#include "cluon/any/any.hpp"

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace cluon {
/**
 * This class provides an instance of a generic message to read values from.
 */
class LIBCLUON_API GenericMessage {
   public:
    GenericMessage() = default;
    GenericMessage(const GenericMessage &) = default;
    GenericMessage(GenericMessage &&) = default;
    GenericMessage &operator=(const GenericMessage &) = default;
    GenericMessage &operator=(GenericMessage &&) = delete;

    void setMetaMessage(const MetaMessage &mm,
                        const std::vector<MetaMessage> &mms,
                        MessageFromProtoDecoder &pd) noexcept;

   private:
    void setData(MessageFromProtoDecoder &pd) noexcept {
        m_data.clear();

        for (const auto &f : m_metaMessage.listOfMetaFields()) {
            if (f.fieldDataType() == MetaMessage::MetaField::BOOL_T) {
                linb::any _v{false};
                auto &v = linb::any_cast<bool&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::CHAR_T) {
                linb::any _v{static_cast<char>('\0')};
                auto &v = linb::any_cast<char&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT8_T) {
                linb::any _v{static_cast<uint8_t>(0)};
                auto &v = linb::any_cast<uint8_t&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT8_T) {
                linb::any _v{static_cast<int8_t>(0)};
                auto &v = linb::any_cast<int8_t&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT16_T) {
                linb::any _v{static_cast<uint16_t>(0)};
                auto &v = linb::any_cast<uint16_t&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT16_T) {
                linb::any _v{static_cast<int16_t>(0)};
                auto &v = linb::any_cast<int16_t&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT32_T) {
                linb::any _v{static_cast<uint32_t>(0)};
                auto &v = linb::any_cast<uint32_t&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT32_T) {
                linb::any _v{static_cast<int32_t>(0)};
                auto &v = linb::any_cast<int32_t&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT64_T) {
                linb::any _v{static_cast<uint64_t>(0)};
                auto &v = linb::any_cast<uint64_t&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT64_T) {
                linb::any _v{static_cast<int64_t>(0)};
                auto &v = linb::any_cast<int64_t&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::FLOAT_T) {
                linb::any _v{static_cast<float>(0.0f)};
                auto &v = linb::any_cast<float&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::DOUBLE_T) {
                linb::any _v{static_cast<double>(0.0)};
                auto &v = linb::any_cast<double&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if ((f.fieldDataType() == MetaMessage::MetaField::STRING_T)
                       || (f.fieldDataType() == MetaMessage::MetaField::BYTES_T)) {
                linb::any _v = std::string{};
                auto &v = linb::any_cast<std::string&>(_v);
                pd.visit(f.fieldIdentifier(), v);
                m_data[f.fieldIdentifier()] = _v;
            } else if (f.fieldDataType() == MetaMessage::MetaField::MESSAGE_T) {
                if (0 < m_mapForScopeOfMetaMessages.count(f.fieldDataTypeName())) {
                    std::string s;
                    pd.visit(f.fieldIdentifier(), s);

                    std::stringstream sstr{s};
                    cluon::MessageFromProtoDecoder protoDecoder;
                    protoDecoder.decodeFrom(sstr);

                    cluon::GenericMessage gm;
                    gm.setMetaMessage(
                        m_mapForScopeOfMetaMessages[f.fieldDataTypeName()], m_scopeOfMetaMessages, protoDecoder);

                    linb::any _v = gm;
                    m_data[f.fieldIdentifier()] = _v;
                }
            }
        }
    }

   public:
    template <class Visitor>
    void accept(Visitor &visitor) {
        visitor.preVisit(m_metaMessage.messageIdentifier(), m_metaMessage.messageName(), m_longName);

        for (const auto &f : m_metaMessage.listOfMetaFields()) {
            if (f.fieldDataType() == MetaMessage::MetaField::BOOL_T) {
                auto &v = linb::any_cast<bool&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::CHAR_T) {
                auto &v = linb::any_cast<char&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT8_T) {
                auto &v = linb::any_cast<uint8_t&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT8_T) {
                auto &v = linb::any_cast<int8_t&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT16_T) {
                auto &v = linb::any_cast<uint16_t&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT16_T) {
                auto &v = linb::any_cast<int16_t&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT32_T) {
                auto &v = linb::any_cast<uint32_t&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT32_T) {
                auto &v = linb::any_cast<int32_t&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT64_T) {
                auto &v = linb::any_cast<uint64_t&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT64_T) {
                auto &v = linb::any_cast<int64_t&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::FLOAT_T) {
                auto &v = linb::any_cast<float&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::DOUBLE_T) {
                auto &v = linb::any_cast<double&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if ((f.fieldDataType() == MetaMessage::MetaField::STRING_T)
                       || (f.fieldDataType() == MetaMessage::MetaField::BYTES_T)) {
                auto &v = linb::any_cast<std::string&>(m_data[f.fieldIdentifier()]);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::MESSAGE_T) {
                if (0 < m_mapForScopeOfMetaMessages.count(f.fieldDataTypeName())) {
                    auto &v = linb::any_cast<cluon::GenericMessage&>(m_data[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                }
            }
        }

        visitor.postVisit();
    }

    template <class PreVisitor, class Visitor, class PostVisitor>
    void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        std::forward<PreVisitor>(preVisit)(m_metaMessage.messageIdentifier(), m_metaMessage.messageName(), m_longName);

        for (const auto &f : m_metaMessage.listOfMetaFields()) {
            if (f.fieldDataType() == MetaMessage::MetaField::BOOL_T) {
                auto &v = linb::any_cast<bool&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::CHAR_T) {
                auto &v = linb::any_cast<char&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT8_T) {
                auto &v = linb::any_cast<uint8_t&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT8_T) {
                auto &v = linb::any_cast<int8_t&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT16_T) {
                auto &v = linb::any_cast<uint16_t&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT16_T) {
                auto &v = linb::any_cast<int16_t&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT32_T) {
                auto &v = linb::any_cast<uint32_t&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT32_T) {
                auto &v = linb::any_cast<int32_t&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT64_T) {
                auto &v = linb::any_cast<uint64_t&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT64_T) {
                auto &v = linb::any_cast<int64_t&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::FLOAT_T) {
                auto &v = linb::any_cast<float&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::DOUBLE_T) {
                auto &v = linb::any_cast<double&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if ((f.fieldDataType() == MetaMessage::MetaField::STRING_T)
                       || (f.fieldDataType() == MetaMessage::MetaField::BYTES_T)) {
                auto &v = linb::any_cast<std::string&>(m_data[f.fieldIdentifier()]);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::MESSAGE_T) {
                if (0 < m_mapForScopeOfMetaMessages.count(f.fieldDataTypeName())) {
                    auto &v = linb::any_cast<cluon::GenericMessage&>(m_data[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          preVisit,
                                          visit,
                                          postVisit);
                }
            }
        }

        std::forward<PostVisitor>(postVisit)();
    }

   private:
    MetaMessage m_metaMessage{};
    std::vector<MetaMessage> m_scopeOfMetaMessages{};
    std::map<std::string, MetaMessage> m_mapForScopeOfMetaMessages{};
    std::string m_longName{""};
    std::map<uint32_t, linb::any> m_data;
};
} // namespace cluon

template <>
struct isVisitable<cluon::GenericMessage> {
    static const bool value = true;
};
template <>
struct isTripletForwardVisitable<cluon::GenericMessage> {
    static const bool value = true;
};
#endif
