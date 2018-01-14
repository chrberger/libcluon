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

#include "cluon/MetaMessageToCPPTransformator.hpp"
#include "cluon/MetaMessage.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>

#include "Mustache/mustache.hpp"

namespace cluon {

const char *headerFileTemplate = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef {{%HEADER_GUARD%}}_HPP
#define {{%HEADER_GUARD%}}_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
{{%NAMESPACE_OPENING%}}
using namespace std::string_literals; // NOLINT
class LIB_API {{%MESSAGE%}} {
    public:
        {{%MESSAGE%}}() = default;
        {{%MESSAGE%}}(const {{%MESSAGE%}}&) = default;
        {{%MESSAGE%}}& operator=(const {{%MESSAGE%}}&) = default;
        {{%MESSAGE%}}({{%MESSAGE%}}&&) noexcept = default; // NOLINT
        {{%MESSAGE%}}& operator=({{%MESSAGE%}}&&) noexcept = default; // NOLINT
        ~{{%MESSAGE%}}() = default;

    public:
        static uint32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        {{#%FIELDS%}}
        {{%MESSAGE%}}& {{%NAME%}}(const {{%TYPE%}} &v) noexcept;
        {{%TYPE%}} {{%NAME%}}() const noexcept;
        {{/%FIELDS%}}

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            {{#%FIELDS%}}
            doVisit({{%FIELDIDENTIFIER%}}, std::move("{{%TYPE%}}"s), std::move("{{%NAME%}}"s), m_{{%NAME%}}, visitor);
            {{/%FIELDS%}}
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            {{#%FIELDS%}}
            doTripletForwardVisit({{%FIELDIDENTIFIER%}}, std::move("{{%TYPE%}}"s), std::move("{{%NAME%}}"s), m_{{%NAME%}}, preVisit, visit, postVisit);
            {{/%FIELDS%}}
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        {{#%FIELDS%}}
        {{%TYPE%}} m_{{%NAME%}}{ {{%FIELD_DEFAULT_INITIALIZATION_VALUE%}}{{%INITIALIZER_SUFFIX%}} }; // field identifier = {{%FIELDIDENTIFIER%}}.
        {{/%FIELDS%}}
};
{{%NAMESPACE_CLOSING%}}

template<>
struct isVisitable<{{%COMPLETEPACKAGENAME_WITH_COLON_SEPARATORS%}}{{%MESSAGE%}}> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<{{%COMPLETEPACKAGENAME_WITH_COLON_SEPARATORS%}}{{%MESSAGE%}}> {
    static const bool value = true;
};
#endif
)";

const char *sourceFileTemplate = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
{{%NAMESPACE_OPENING%}}

uint32_t {{%MESSAGE%}}::ID() {
    return {{%IDENTIFIER%}};
}

const std::string {{%MESSAGE%}}::ShortName() {
    return "{{%MESSAGE%}}";
}
const std::string {{%MESSAGE%}}::LongName() {
    return "{{%COMPLETEPACKAGENAME%}}{{%MESSAGE%}}";
}
{{#%FIELDS%}}
{{%MESSAGE%}}& {{%MESSAGE%}}::{{%NAME%}}(const {{%TYPE%}} &v) noexcept {
    m_{{%NAME%}} = v;
    return *this;
}
{{%TYPE%}} {{%MESSAGE%}}::{{%NAME%}}() const noexcept {
    return m_{{%NAME%}};
}
{{/%FIELDS%}}
{{%NAMESPACE_CLOSING%}}
)";

std::string MetaMessageToCPPTransformator::contentHeader() noexcept {
    m_dataToBeRendered.set("%FIELDS%", m_fields);

    kainjow::mustache::mustache tmpl{headerFileTemplate};
    // Reset Mustache's default string-escaper.
    tmpl.set_custom_escape([](const std::string &s) { return s; });
    std::stringstream sstr;
    sstr << tmpl.render(m_dataToBeRendered);
    const std::string str(sstr.str());
    return str;
}

std::string MetaMessageToCPPTransformator::contentSource() noexcept {
    m_dataToBeRendered.set("%FIELDS%", m_fields);

    kainjow::mustache::mustache tmpl{sourceFileTemplate};
    tmpl.set_custom_escape([](const std::string &s) { return s; });
    std::stringstream sstr;
    sstr << tmpl.render(m_dataToBeRendered);
    const std::string str(sstr.str());
    return str;
}

void MetaMessageToCPPTransformator::visit(const MetaMessage &mm) noexcept {
    kainjow::mustache::data dataToBeRendered;
    kainjow::mustache::data fields{kainjow::mustache::data::type::list};

    try {
        std::map<MetaMessage::MetaField::MetaFieldDataTypes, std::string> typeToTypeStringMap = {
            {MetaMessage::MetaField::BOOL_T, "bool"},
            {MetaMessage::MetaField::CHAR_T, "char"},
            {MetaMessage::MetaField::UINT8_T, "uint8_t"},
            {MetaMessage::MetaField::INT8_T, "int8_t"},
            {MetaMessage::MetaField::UINT16_T, "uint16_t"},
            {MetaMessage::MetaField::INT16_T, "int16_t"},
            {MetaMessage::MetaField::UINT32_T, "uint32_t"},
            {MetaMessage::MetaField::INT32_T, "int32_t"},
            {MetaMessage::MetaField::UINT64_T, "uint64_t"},
            {MetaMessage::MetaField::INT64_T, "int64_t"},
            {MetaMessage::MetaField::FLOAT_T, "float"},
            {MetaMessage::MetaField::DOUBLE_T, "double"},
            {MetaMessage::MetaField::STRING_T, "std::string"},
            {MetaMessage::MetaField::BYTES_T, "std::string"},
        };

        std::map<MetaMessage::MetaField::MetaFieldDataTypes, std::string> typeToDefaultInitizationValueMap = {
            {MetaMessage::MetaField::BOOL_T, "false"},
            {MetaMessage::MetaField::CHAR_T, "'\\0'"},
            {MetaMessage::MetaField::UINT8_T, "0"},
            {MetaMessage::MetaField::INT8_T, "0"},
            {MetaMessage::MetaField::UINT16_T, "0"},
            {MetaMessage::MetaField::INT16_T, "0"},
            {MetaMessage::MetaField::UINT32_T, "0"},
            {MetaMessage::MetaField::INT32_T, "0"},
            {MetaMessage::MetaField::UINT64_T, "0"},
            {MetaMessage::MetaField::INT64_T, "0"},
            {MetaMessage::MetaField::FLOAT_T, "0.0"},
            {MetaMessage::MetaField::DOUBLE_T, "0.0"},
            {MetaMessage::MetaField::STRING_T, R"("")"},
            {MetaMessage::MetaField::BYTES_T, R"("")"},
        };

        std::string namespacePrefix;
        std::string messageName{mm.messageName()};
        const auto pos = mm.messageName().find_last_of('.');
        if (std::string::npos != pos) {
            namespacePrefix = mm.messageName().substr(0, pos);
            messageName     = mm.messageName().substr(pos + 1);
        }

        const std::string completePackageName
            = mm.packageName() + (!mm.packageName().empty() && !namespacePrefix.empty() ? "." : "") + namespacePrefix;
        const std::string completePackageNameWithColonSeparators{
            std::regex_replace(completePackageName, std::regex("\\."), "::")}; // NOLINT
        const std::string namespaceHeader{
            std::regex_replace(completePackageName, std::regex("\\."), " { namespace ")}; // NOLINT
        const std::string namespaceFooter(
            static_cast<uint32_t>(std::count(std::begin(namespaceHeader), std::end(namespaceHeader), '{'))
                + (!namespaceHeader.empty() ? 1 : 0),
            '}');
        std::string headerGuard{std::regex_replace(completePackageName, std::regex("\\."), "_")}; // NOLINT
        headerGuard += (!headerGuard.empty() ? +"_" : "") + messageName;
        std::transform(std::begin(headerGuard), std::end(headerGuard), std::begin(headerGuard), [](unsigned char c) {
            return ::toupper(c);
        });

        dataToBeRendered.set("%HEADER_GUARD%", headerGuard);
        dataToBeRendered.set("%NAMESPACE_OPENING%",
                             (!namespaceHeader.empty() ? "namespace " + namespaceHeader + " {" : ""));
        dataToBeRendered.set("%COMPLETEPACKAGENAME%", completePackageName + (!completePackageName.empty() ? "." : ""));
        dataToBeRendered.set("%COMPLETEPACKAGENAME_WITH_COLON_SEPARATORS%",
                             completePackageNameWithColonSeparators
                                 + (!completePackageNameWithColonSeparators.empty() ? "::" : ""));
        dataToBeRendered.set("%MESSAGE%", messageName);
        dataToBeRendered.set("%NAMESPACE_CLOSING%", namespaceFooter);
        dataToBeRendered.set("%IDENTIFIER%", std::to_string(mm.messageIdentifier()));

        for (const auto &e : mm.listOfMetaFields()) {
            std::string fieldName{std::regex_replace(e.fieldName(), std::regex("\\."), "_")}; // NOLINT
            kainjow::mustache::data fieldEntry;
            fieldEntry.set("%NAME%", fieldName);
            if (MetaMessage::MetaField::MESSAGE_T != e.fieldDataType()) {
                fieldEntry.set("%TYPE%", typeToTypeStringMap[e.fieldDataType()]);

                const std::string defaultInitializatioValue{(e.defaultInitializationValue().empty()
                                                                 ? typeToDefaultInitizationValueMap[e.fieldDataType()]
                                                                 : e.defaultInitializationValue())};
                fieldEntry.set("%FIELD_DEFAULT_INITIALIZATION_VALUE%", defaultInitializatioValue);

                std::string initializerSuffix;
                if (e.fieldDataType() == MetaMessage::MetaField::FLOAT_T) {
                    initializerSuffix = "f"; // suffix for float types.
                } else if (e.fieldDataType() == MetaMessage::MetaField::STRING_T
                           || e.fieldDataType() == MetaMessage::MetaField::BYTES_T) {
                    initializerSuffix = "s"; // suffix to enforce std::string initialization.
                }
                fieldEntry.set("%INITIALIZER_SUFFIX%", initializerSuffix);
            } else {
                const std::string tmp{mm.packageName() + (!mm.packageName().empty() ? "." : "")
                                      + e.fieldDataTypeName()};
                const std::string completeDataTypeNameWithDoubleColons{
                    std::regex_replace(tmp, std::regex("\\."), "::")}; // NOLINT

                fieldEntry.set("%TYPE%", completeDataTypeNameWithDoubleColons);
            }
            fieldEntry.set("%FIELDIDENTIFIER%", std::to_string(e.fieldIdentifier()));

            fields.push_back(fieldEntry);
        }
    } catch (std::regex_error &) { // LCOV_EXCL_LINE
    }

    m_dataToBeRendered = std::move(dataToBeRendered);
    m_fields           = std::move(fields);
}
} // namespace cluon
