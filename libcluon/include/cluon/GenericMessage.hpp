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

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace cluon {
/**
 * This class provides an instance of a generic message to read values from.
 */
class LIBCLUON_API GenericMessage {
   private:
    GenericMessage(GenericMessage &&) = delete;
    GenericMessage &operator=(const GenericMessage &) = delete;
    GenericMessage &operator=(GenericMessage &&) = delete;

   public:
    GenericMessage()                       = default;
    GenericMessage(const GenericMessage &) = default;

    void setMetaMessage(const MetaMessage &mm,
                        const std::vector<MetaMessage> &mms,
                        const MessageFromProtoDecoder &pd) noexcept;

    template <class Visitor>
    void accept(Visitor &visitor) {
        visitor.preVisit(m_metaMessage.messageIdentifier(), m_metaMessage.messageName(), m_longName);

        for (const auto &f : m_metaMessage.listOfMetaFields()) {
            if (f.fieldDataType() == MetaMessage::MetaField::BOOL_T) {
                bool v{false};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::CHAR_T) {
                char v{'\0'};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT8_T) {
                uint8_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT8_T) {
                int8_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT16_T) {
                uint16_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT16_T) {
                int16_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT32_T) {
                uint32_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT32_T) {
                int32_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT64_T) {
                uint64_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT64_T) {
                int64_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::FLOAT_T) {
                float v{0.0f};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::DOUBLE_T) {
                double v{0.0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if ((f.fieldDataType() == MetaMessage::MetaField::STRING_T)
                       || (f.fieldDataType() == MetaMessage::MetaField::BYTES_T)) {
                std::string v{""};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doVisit(f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
            } else if (f.fieldDataType() == MetaMessage::MetaField::MESSAGE_T) {
                if (0 < m_mapForScopeOfMetaMessages.count(f.fieldDataTypeName())) {
                    std::string s;
                    m_protoDecoder.visit(f.fieldIdentifier(), s);

                    std::stringstream sstr{s};
                    cluon::MessageFromProtoDecoder protoDecoder;
                    protoDecoder.decodeFrom(sstr);

                    cluon::GenericMessage gm;
                    gm.setMetaMessage(
                        m_mapForScopeOfMetaMessages[f.fieldDataTypeName()], m_scopeOfMetaMessages, protoDecoder);

                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), gm, visitor);
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
                bool v{false};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::CHAR_T) {
                char v{'\0'};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT8_T) {
                uint8_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT8_T) {
                int8_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT16_T) {
                uint16_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT16_T) {
                int16_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT32_T) {
                uint32_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT32_T) {
                int32_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT64_T) {
                uint64_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT64_T) {
                int64_t v{0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::FLOAT_T) {
                float v{0.0f};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::DOUBLE_T) {
                double v{0.0};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if ((f.fieldDataType() == MetaMessage::MetaField::STRING_T)
                       || (f.fieldDataType() == MetaMessage::MetaField::BYTES_T)) {
                std::string v{""};
                m_protoDecoder.visit(f.fieldIdentifier(), v);
                doTripletForwardVisit(f.fieldIdentifier(),
                                      std::move(f.fieldDataTypeName()),
                                      std::move(f.fieldName()),
                                      v,
                                      preVisit,
                                      visit,
                                      postVisit);
            } else if (f.fieldDataType() == MetaMessage::MetaField::MESSAGE_T) {
                if (0 < m_mapForScopeOfMetaMessages.count(f.fieldDataTypeName())) {
                    std::string s;
                    m_protoDecoder.visit(f.fieldIdentifier(), s);

                    std::stringstream sstr{s};
                    cluon::MessageFromProtoDecoder protoDecoder;
                    protoDecoder.decodeFrom(sstr);

                    cluon::GenericMessage gm;
                    gm.setMetaMessage(
                        m_mapForScopeOfMetaMessages[f.fieldDataTypeName()], m_scopeOfMetaMessages, protoDecoder);

                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          gm,
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
    MessageFromProtoDecoder m_protoDecoder{};
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
