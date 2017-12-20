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

#include "cluon/MetaMessageToProtoTransformator.hpp"
#include "cluon/MetaMessage.hpp"

#include <algorithm>
#include <map>
#include <sstream>
#include <string>

#include "Mustache/mustache.hpp"

namespace cluon {

const char *proto2FileTemplate = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//

{{%PROTO2_HEADER%}}

// Message identifier: {{%MESSAGE_IDENTIFIER%}}.
message {{%MESSAGE%}} {
    {{#%FIELDS%}}
    optional {{%TYPE%}} {{%NAME%}} = {{%FIELD_IDENTIFIER%}};
    {{/%FIELDS%}}
}
)";

std::string MetaMessageToProtoTransformator::content(bool withProtoHeader) noexcept {
    if (withProtoHeader) {
        m_dataToBeRendered.set("%PROTO2_HEADER%", R"(
// This line is only needed when using Google Protobuf 3.
syntax = "proto2";
)");
    }
    m_dataToBeRendered.set("%FIELDS%", m_fields);

    kainjow::mustache::mustache tmpl{proto2FileTemplate};
    // Reset Mustache's default string-escaper.
    tmpl.set_custom_escape([](const std::string &s) { return s; });
    std::stringstream sstr;
    sstr << tmpl.render(m_dataToBeRendered);
    const std::string str(sstr.str());
    return str;
}

void MetaMessageToProtoTransformator::visit(const MetaMessage &mm) noexcept {
    kainjow::mustache::data dataToBeRendered;
    kainjow::mustache::data fields{kainjow::mustache::data::type::list};

    std::map<MetaMessage::MetaField::MetaFieldDataTypes, std::string> typeToTypeStringMap = {
        {MetaMessage::MetaField::BOOL_T, "bool"},
        {MetaMessage::MetaField::CHAR_T, "sint32"},
        {MetaMessage::MetaField::UINT8_T, "uint32"},
        {MetaMessage::MetaField::INT8_T, "sint32"},
        {MetaMessage::MetaField::UINT16_T, "uint32"},
        {MetaMessage::MetaField::INT16_T, "sint32"},
        {MetaMessage::MetaField::UINT32_T, "uint32"},
        {MetaMessage::MetaField::INT32_T, "sint32"},
        {MetaMessage::MetaField::UINT64_T, "uint64"},
        {MetaMessage::MetaField::INT64_T, "sint64"},
        {MetaMessage::MetaField::FLOAT_T, "float"},
        {MetaMessage::MetaField::DOUBLE_T, "double"},
        {MetaMessage::MetaField::STRING_T, "string"},
        {MetaMessage::MetaField::BYTES_T, "bytes"},
    };

    std::string namespacePrefix;
    std::string messageName{mm.messageName()};
    const auto pos = mm.messageName().find_last_of('.');
    if (std::string::npos != pos) {
        namespacePrefix = mm.messageName().substr(0, pos);
        messageName     = mm.messageName().substr(pos + 1);
    }

    std::string packageNameWithUnderscores{mm.packageName()};
    std::replace(packageNameWithUnderscores.begin(), packageNameWithUnderscores.end(), '.', '_');
    const std::string completePackageNameWithNamespacePrefix
        = packageNameWithUnderscores + (!packageNameWithUnderscores.empty() && !namespacePrefix.empty() ? "." : "")
          + namespacePrefix;

    std::string completePackageNameWithNamespacePrefixWithUnderscores{completePackageNameWithNamespacePrefix};
    std::replace(completePackageNameWithNamespacePrefixWithUnderscores.begin(), completePackageNameWithNamespacePrefixWithUnderscores.end(), '.', '_');
    const std::string completeMessageNameWithUnderscores
        = completePackageNameWithNamespacePrefixWithUnderscores
          + (!completePackageNameWithNamespacePrefixWithUnderscores.empty() ? +"_" : "") + messageName;

    dataToBeRendered.set("%MESSAGE_IDENTIFIER%", std::to_string(mm.messageIdentifier()));
    dataToBeRendered.set("%MESSAGE%", completeMessageNameWithUnderscores);

    for (const auto &f : mm.listOfMetaFields()) {
        std::string fieldName{f.fieldName()};
        std::replace(fieldName.begin(), fieldName.end(), '.', '_');

        kainjow::mustache::data fieldEntry;
        fieldEntry.set("%NAME%", fieldName);
        if (MetaMessage::MetaField::MESSAGE_T != f.fieldDataType()) {
            fieldEntry.set("%TYPE%", typeToTypeStringMap[f.fieldDataType()]);
        } else {
            std::string dataTypeNameWithUnderscores{f.fieldDataTypeName()};
            std::replace(dataTypeNameWithUnderscores.begin(), dataTypeNameWithUnderscores.end(), '.', '_');
            std::string tmp{packageNameWithUnderscores + (!packageNameWithUnderscores.empty() ? "_" : "")};
            tmp += dataTypeNameWithUnderscores;
            fieldEntry.set("%TYPE%", tmp);
        }
        fieldEntry.set("%FIELD_IDENTIFIER%", std::to_string(f.fieldIdentifier()));
        fields.push_back(fieldEntry);
    }

    m_dataToBeRendered = std::move(dataToBeRendered);
    m_fields           = std::move(fields);
}
} // namespace cluon
