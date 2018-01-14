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
#include "cluon/any/any.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/MessageFromProtoDecoder.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

//#include "cluon/cluonDataStructures.hpp"
//#include <utility>

namespace cluon {
/**
GenericMessage is providing an abstraction level to work with concrete
messages. Therefore, it is acting as both, a Visitor to turn concrete
messages into GenericMessages or as Visitable to access the contained
data. GenericMessage would use C++'s std::any type; to allow C++14
compilers, we use the backport from linb::any.

Creating a GenericMessage:
There are several ways to create a GenericMessage. The first option is to
provide a message specification in ODVD format as result from MessageParser
next to a serialized byte sequence in Proto format:

1) This example demonstrates how to process a given message specification
   to decode a Proto-encoded byte sequence (in protoEncodedData). The
   message specification is given in messageSpecification that is parsed
   from MessageParser. On success, it is tried to decode the Proto-encoded
   data into a GenericMesssage representing an instance of "MyMessage".

   Using a message specification that does not match the serialized Proto
   data might result in unexpected behavior.

\code{.cpp}
// protoEncodedData is provided from somewhere, i.e., via network for example
std::string protoEncodedData = <...>
std::stringstream sstr{protoEncodedData};
cluon::MessageFromProtoDecoder protoDecoder;
protoDecoder.decodeFrom(sstr);

const char *messageSpecification = R"(
message MyMessage [id = 123] {
   int32 field1 [id = 1];
   int32 field2 [id = 2];
}

cluon::MessageParser mp;
auto retVal = mp.parse(std::string(messageSpecification));
if (cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second) {
    cluon::GenericMessage gm;
    auto listOfMetaMessages = retVal.first;
    gm.createFrom(listOfMetaMessages[0], listOfMetaMessages, protoDecoder);
}
\endcode


2) This example demonstrates how to turn a given concrete message into a
   GenericMessage. Afterwards, the GenericMessage can be post-processed
   with Visitors.


\code{.cpp}
MyMessage msg;
// set some fields in msg.

cluon::GenericMessage gm;
gm.createFrom<MyMessage>(msg);
\endcode


After an instance of GenericMessage is available, it can be post-processed
into various representations:

1) Printing the contained data ("toString"; GenericMessage is being visited):

\code{.cpp}
GenericMessage gm;
// gm is created using one of the aforementioned options.

std::stringstream buffer;
gm.accept([](uint32_t, const std::string &, const std::string &) {},
          [&buffer](uint32_t, std::string &&, std::string &&n, auto v) { buffer << n << " = " << v << std::endl; },
          []() {});
std::cout << buffer.str() << std::endl;
\endcode


2) Filling the values of another concrete message (GenericMessage is
   acting as Visitor to another message):

\code{.cpp}
GenericMessage gm;
// gm is created using one of the aforementioned options.

Message msg;
msg.accept(gm);
\endcode


3) Serialize the GenericMessage gm into a Proto-format:

\code{.cpp}
GenericMessage gm;
// gm is created using one of the aforementioned options.

cluon::MessageToProtoEncoder protoEncoder;
gm.accept(protoEncoder);
const std::string{protoEncoder.encodedData()};
\endcode


4) Serialize the GenericMessage gm into JSON:

\code{.cpp}
GenericMessage gm;
// gm is created using one of the aforementioned options.

cluon::JSONVisitor j;
gm.accept(j);
std::cout << j.json();
\endcode


4) Dynamically transforming a given Proto-encoded byte sequence into JSON
   at runtime:

\code{.cpp}
// protoEncodedData is provided from somewhere, i.e., via network for example
std::string protoEncodedData = <...>
std::stringstream sstr{protoEncodedData};
cluon::MessageFromProtoDecoder protoDecoder;
protoDecoder.decodeFrom(sstr);

const char *messageSpecification = R"(
message MyMessage [id = 123] {
   int32 field1 [id = 1];
   int32 field2 [id = 2];
}

cluon::MessageParser mp;
auto retVal = mp.parse(std::string(messageSpecification));
if (cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second) {
    cluon::GenericMessage gm;
    auto listOfMetaMessages = retVal.first;
    gm.createFrom(listOfMetaMessages[0], listOfMetaMessages, protoDecoder);
}

cluon::JSONVisitor j;
gm.accept(j);
std::cout << j.json();
\endcode
*/
class LIBCLUON_API GenericMessage {
   private:
    class GenericMessageVisitor {
       private:
        GenericMessageVisitor(const GenericMessageVisitor &) = delete;
        GenericMessageVisitor(GenericMessageVisitor &&)      = delete;
        GenericMessageVisitor &operator=(const GenericMessageVisitor &) = delete;
        GenericMessageVisitor &operator=(GenericMessageVisitor &&) = delete;

       public:
        GenericMessageVisitor() = default;

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
            cluon::MetaMessage::MetaField mf;
            mf.fieldIdentifier(id)
                .fieldDataType(cluon::MetaMessage::MetaField::MESSAGE_T)
                .fieldDataTypeName(typeName)
                .fieldName(name);

            GenericMessage gm;
            gm.createFrom<T>(value);

            m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{gm};
            m_metaMessage.add(std::move(mf));
        }

       public:
        /**
         * @return MetaMessage for this GenericMessage.
         */
        MetaMessage metaMessage() const noexcept;

        /**
         * @return Intermediate data representation for this GenericMessage.
         */
        std::map<uint32_t, linb::any> intermediateDataRepresentation() const noexcept;

       private:
        MetaMessage m_metaMessage{};
        std::map<uint32_t, linb::any> m_intermediateDataRepresentation;
    };

   private:
    GenericMessage &operator=(GenericMessage &&) = delete;

   public:
    GenericMessage()                       = default;
    GenericMessage(GenericMessage &&)      = default;
    GenericMessage(const GenericMessage &) = default;
    GenericMessage &operator=(const GenericMessage &) = default;

    /**
     * This methods creates this GenericMessage from a given concrete message.
     *
     * @param msg Concrete message used to derive this GenericMessage from.
     */
    template <typename T>
    void createFrom(T &msg) {
        GenericMessageVisitor gmv;
        msg.accept(gmv);

        m_metaMessage = gmv.metaMessage();
        m_intermediateDataRepresentation.clear();
        m_intermediateDataRepresentation = gmv.intermediateDataRepresentation();
    }

    /**
     * This method creates an empty GenericMessage from a given message
     * specification parsed from MessageParser.
     *
     * @param mm MetaMessage describing the fields for the message to be resolved.
     * @param mms List of MetaMessages that are known (used for resolving nested message).
     */
    void createFrom(const MetaMessage &mm, const std::vector<MetaMessage> &mms) noexcept;

    /**
     * This method creates this GenericMessage from a serialized representation
     * in Proto-format and a given message specification parsed from MessageParser.
     *
     * @param mm MetaMessage describing the fields for the message to be resolved.
     * @param mms List of MetaMessages that are known (used for resolving nested message).
     * @param pd ProtoDecoder containing the values to be decoded.
     */
    void createFrom(const MetaMessage &mm, const std::vector<MetaMessage> &mms, MessageFromProtoDecoder &pd) noexcept;

   private:
    /**
     * This method creates the intermediate data representation from the
     * Proto-encoded data contained in pd.
     *
     * @param pd ProtoDecoder containing the decoded Proto fields.
     */
    void createIntermediateDataRepresentationFrom(MessageFromProtoDecoder &pd) noexcept {
        m_intermediateDataRepresentation.clear();
        for (const auto &f : m_metaMessage.listOfMetaFields()) {
            if (f.fieldDataType() == MetaMessage::MetaField::BOOL_T) {
                try {
                    linb::any _v{false};
                    auto &v = linb::any_cast<bool &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::CHAR_T) {
                try {
                    linb::any _v{static_cast<char>('\0')};
                    auto &v = linb::any_cast<char &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT8_T) {
                try {
                    linb::any _v{static_cast<uint8_t>(0)};
                    auto &v = linb::any_cast<uint8_t &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT8_T) {
                try {
                    linb::any _v{static_cast<int8_t>(0)};
                    auto &v = linb::any_cast<int8_t &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT16_T) {
                try {
                    linb::any _v{static_cast<uint16_t>(0)};
                    auto &v = linb::any_cast<uint16_t &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT16_T) {
                try {
                    linb::any _v{static_cast<int16_t>(0)};
                    auto &v = linb::any_cast<int16_t &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT32_T) {
                try {
                    linb::any _v{static_cast<uint32_t>(0)};
                    auto &v = linb::any_cast<uint32_t &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT32_T) {
                try {
                    linb::any _v{static_cast<int32_t>(0)};
                    auto &v = linb::any_cast<int32_t &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT64_T) {
                try {
                    linb::any _v{static_cast<uint64_t>(0)};
                    auto &v = linb::any_cast<uint64_t &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT64_T) {
                try {
                    linb::any _v{static_cast<int64_t>(0)};
                    auto &v = linb::any_cast<int64_t &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::FLOAT_T) {
                try {
                    linb::any _v{static_cast<float>(0.0f)};
                    auto &v = linb::any_cast<float &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::DOUBLE_T) {
                try {
                    linb::any _v{static_cast<double>(0.0)};
                    auto &v = linb::any_cast<double &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if ((f.fieldDataType() == MetaMessage::MetaField::STRING_T)
                       || (f.fieldDataType() == MetaMessage::MetaField::BYTES_T)) {
                try {
                    linb::any _v = std::string{};
                    auto &v      = linb::any_cast<std::string &>(_v);
                    pd.visit(f.fieldIdentifier(), "", "", v);
                    m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::MESSAGE_T) {
                if (0 < m_mapForScopeOfMetaMessages.count(f.fieldDataTypeName())) {
                    std::string s;
                    pd.visit(f.fieldIdentifier(), "", "", s);

                    // Create a nested ProtoDecoder for the contained complex message.
                    std::stringstream sstr{s};
                    cluon::MessageFromProtoDecoder protoDecoder;
                    protoDecoder.decodeFrom(sstr);

                    // Create a GenericMessage from the decoded Proto-data.
                    cluon::GenericMessage gm;
                    gm.createFrom(
                        m_mapForScopeOfMetaMessages[f.fieldDataTypeName()], m_scopeOfMetaMessages, protoDecoder);

                    m_intermediateDataRepresentation[f.fieldIdentifier()] = linb::any{gm};
                }
            }
        }
    }

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
        (void)typeName;
        (void)name;
        if (0 < m_intermediateDataRepresentation.count(id)) {
            try {
                auto &v = linb::any_cast<cluon::GenericMessage &>(m_intermediateDataRepresentation[id]);
                value.accept(v);
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        }
    }

   public:
    /**
     * This method allows other instances to visit this GenericMessage for
     * post-processing the contained data.
     *
     * @param visitor Instance of the visitor visiting this GenericMessage.
     */
    template <class Visitor>
    void accept(Visitor &visitor) {
        visitor.preVisit(m_metaMessage.messageIdentifier(), m_metaMessage.messageName(), m_longName);

        for (const auto &f : m_metaMessage.listOfMetaFields()) {
            if (f.fieldDataType() == MetaMessage::MetaField::BOOL_T
                && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<bool &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::CHAR_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<char &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT8_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<uint8_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT8_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<int8_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT16_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<uint16_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT16_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<int16_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT32_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<uint32_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT32_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<int32_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT64_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<uint64_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT64_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<int64_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::FLOAT_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<float &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::DOUBLE_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<double &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (((f.fieldDataType() == MetaMessage::MetaField::STRING_T)
                        || (f.fieldDataType() == MetaMessage::MetaField::BYTES_T))
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<std::string &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::MESSAGE_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<cluon::GenericMessage &>(
                        m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doVisit(
                        f.fieldIdentifier(), std::move(f.fieldDataTypeName()), std::move(f.fieldName()), v, visitor);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            }
        }

        visitor.postVisit();
    }

    /**
     * This method allows other instances to visit this GenericMessage for
     * post-processing the contained data.
     *
     * @param _preVisit Instance of the visitor pre-visiting this GenericMessage.
     * @param _visit Instance of the visitor visiting this GenericMessage.
     * @param _postVisit Instance of the visitor post-visiting this GenericMessage.
     */
    template <class PreVisitor, class Visitor, class PostVisitor>
    void accept(PreVisitor &&_preVisit, Visitor &&_visit, PostVisitor &&_postVisit) {
        std::forward<PreVisitor>(_preVisit)(m_metaMessage.messageIdentifier(), m_metaMessage.messageName(), m_longName);

        for (const auto &f : m_metaMessage.listOfMetaFields()) {
            if (f.fieldDataType() == MetaMessage::MetaField::BOOL_T
                && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<bool &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::CHAR_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<char &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT8_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<uint8_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT8_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<int8_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT16_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<uint16_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT16_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<int16_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT32_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<uint32_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT32_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<int32_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::UINT64_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<uint64_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::INT64_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<int64_t &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::FLOAT_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<float &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::DOUBLE_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<double &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (((f.fieldDataType() == MetaMessage::MetaField::STRING_T)
                        || (f.fieldDataType() == MetaMessage::MetaField::BYTES_T))
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<std::string &>(m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            } else if (f.fieldDataType() == MetaMessage::MetaField::MESSAGE_T
                       && (0 < m_intermediateDataRepresentation.count(f.fieldIdentifier()))) {
                try {
                    auto &v = linb::any_cast<cluon::GenericMessage &>(
                        m_intermediateDataRepresentation[f.fieldIdentifier()]);
                    doTripletForwardVisit(f.fieldIdentifier(),
                                          std::move(f.fieldDataTypeName()),
                                          std::move(f.fieldName()),
                                          v,
                                          _preVisit,
                                          _visit,
                                          _postVisit);
                } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
                }
            }
        }

        std::forward<PostVisitor>(_postVisit)();
    }

   private:
    MetaMessage m_metaMessage{};
    std::vector<MetaMessage> m_scopeOfMetaMessages{};
    std::map<std::string, MetaMessage> m_mapForScopeOfMetaMessages{};
    std::string m_longName{""};
    std::map<uint32_t, linb::any> m_intermediateDataRepresentation;
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
