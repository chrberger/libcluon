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

#include "cluon/MessageParser.hpp"
#include "cluon/MetaMessage.hpp"
#include "cpp-peglib/peglib.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

namespace cluon {

std::pair<std::vector<MetaMessage>, MessageParser::MessageParserErrorCodes>
MessageParser::parse(const std::string &input) {
    const char *grammarMessageSpecificationLanguage = R"(
        MESSAGES_SPECIFICATION      <- PACKAGE_DECLARATION? MESSAGE_DECLARATION*
        PACKAGE_DECLARATION         <- 'package' PACKAGE_IDENTIFIER ';'
        PACKAGE_IDENTIFIER          <- < IDENTIFIER ('.' IDENTIFIER)* >

        MESSAGE_DECLARATION         <- 'message' MESSAGE_IDENTIFIER MESSAGE_OPTIONS '{' FIELD* '}'
        MESSAGE_IDENTIFIER          <- < IDENTIFIER ('.' IDENTIFIER)* >
        MESSAGE_OPTIONS             <- '[' 'id' '=' NATURAL_NUMBER ','? ']'

        FIELD                       <- PRIMITIVE_FIELD

        PRIMITIVE_FIELD             <- PRIMITIVE_TYPE IDENTIFIER ('[' PRIMITIVE_FIELD_OPTIONS ']')? ';'
        PRIMITIVE_FIELD_OPTIONS     <- PRIMITIVE_FIELD_DEFAULT? ','? NUMERICAL_FIELD_IDENTIFIER?
        NUMERICAL_FIELD_IDENTIFIER  <- 'id' '=' NATURAL_NUMBER
        PRIMITIVE_FIELD_DEFAULT     <- 'default' '=' (FLOAT_NUMBER / BOOL / CHARACTER / STRING)
        PRIMITIVE_TYPE              <- < 'bool' / 'float' / 'double' /
                                         'char' /
                                         'bytes' / 'string' /
                                         'int8' / 'uint8' / 
                                         'int16' / 'uint16' / 
                                         'int32' / 'uint32' / 
                                         'int64' / 'uint64' /
                                         MESSAGE_TYPE >

        MESSAGE_TYPE                <- < IDENTIFIER ('.' IDENTIFIER)* >

        IDENTIFIER                  <- < [a-zA-Z][a-zA-Z0-9]* >
        DIGIT                       <- < [0-9] >
        NATURAL_NUMBER              <- < [1-9] DIGIT* >
        FLOAT_NUMBER                <- < ('+' / '-')? DIGIT DIGIT* (('.') DIGIT*)? >
        BOOL                        <- < 'true' > / < 'false' >
        STRING                      <- '"' < (!'"'.)* > '"'
        CHARACTER                   <- '\'' < (!'\'' .) > '\''

        %whitespace                 <- [ \t\r\n]*
    )";

    ////////////////////////////////////////////////////////////////////////////

    // Function to check for unique field names.
    std::function<bool(const peg::Ast &,
                       std::string &,
                       std::vector<std::string> &,
                       std::vector<std::string> &,
                       std::vector<int32_t> &,
                       std::vector<int32_t> &)>
        check4UniqueFieldNames
        = [&checkForUniqueFieldNames = check4UniqueFieldNames](const peg::Ast &ast,
                                                               std::string &prefix,
                                                               std::vector<std::string> &messageNames,
                                                               std::vector<std::string> &fieldNames,
                                                               std::vector<int32_t> &numericalMessageIdentifiers,
                                                               std::vector<int32_t> &numericalFieldIdentifiers) {
        bool retVal = true;
        // First, we need to visit the children of AST node MESSAGES_SPECIFICATION.
        if ("MESSAGES_SPECIFICATION" == ast.name) {
            for (const auto &node : ast.nodes) {
                retVal &= checkForUniqueFieldNames(
                    *node, prefix, messageNames, fieldNames, numericalMessageIdentifiers, numericalFieldIdentifiers);
            }
            // Try finding duplicated message identifiers.
            if (retVal) {
                std::sort(std::begin(numericalMessageIdentifiers), std::end(numericalMessageIdentifiers));
                int32_t duplicatedMessageIdentifier{-1};
                for (auto it{std::begin(numericalMessageIdentifiers)}; it != std::end(numericalMessageIdentifiers);
                     it++) {
                    if (it + 1 != std::end(numericalMessageIdentifiers)) {
                        if (std::find(it + 1, std::end(numericalMessageIdentifiers), *it)
                            != std::end(numericalMessageIdentifiers)) {
                            duplicatedMessageIdentifier = *it;
                        }
                    }
                }
                retVal &= (-1 == duplicatedMessageIdentifier);
                if (!retVal) {
                    std::cerr << "[cluon::MessageParser] Found duplicated numerical message identifier: "
                              << duplicatedMessageIdentifier << '\n';
                }
            }
            // Try finding duplicated message names.
            if (retVal) {
                std::sort(std::begin(messageNames), std::end(messageNames));
                std::string duplicatedMessageName;
                for (auto it{std::begin(messageNames)}; it != std::end(messageNames); it++) {
                    if (it + 1 != std::end(messageNames)) {
                        if (std::find(it + 1, std::end(messageNames), *it) != std::end(messageNames)) {
                            duplicatedMessageName = *it;
                        }
                    }
                }
                retVal &= (duplicatedMessageName.empty());
                if (!retVal) {
                    std::cerr << "[cluon::MessageParser] Found duplicated message name '" << duplicatedMessageName
                              << "'" << '\n';
                }
            }
        }
        // Second, we need to visit the children of AST node MESSAGE_DECLARATION.
        if ("MESSAGE_DECLARATION" == ast.name) {
            fieldNames.clear();
            numericalFieldIdentifiers.clear();
            prefix = "";
            retVal = true;

            for (const auto &node : ast.nodes) {
                if ("MESSAGE_IDENTIFIER" == node->name) {
                    prefix = node->token;
                    messageNames.push_back(trim(prefix));
                } else if ("NATURAL_NUMBER" == node->name) {
                    numericalMessageIdentifiers.push_back(std::stoi(node->token));
                } else if ("PRIMITIVE_FIELD" == node->name) {
                    retVal &= checkForUniqueFieldNames(*node,
                                                       prefix,
                                                       messageNames,
                                                       fieldNames,
                                                       numericalMessageIdentifiers,
                                                       numericalFieldIdentifiers);
                }
            }

            // Try finding duplicated numerical field identifiers.
            if (retVal) {
                std::sort(std::begin(numericalFieldIdentifiers), std::end(numericalFieldIdentifiers));
                int32_t duplicatedFieldIdentifier{-1};
                for (auto it{std::begin(numericalFieldIdentifiers)}; it != std::end(numericalFieldIdentifiers); it++) {
                    if (it + 1 != std::end(numericalFieldIdentifiers)) {
                        if (std::find(it + 1, std::end(numericalFieldIdentifiers), *it)
                            != std::end(numericalFieldIdentifiers)) {
                            duplicatedFieldIdentifier = *it;
                        }
                    }
                }
                retVal &= (-1 == duplicatedFieldIdentifier);
                if (!retVal) {
                    std::cerr << "[cluon::MessageParser] Found duplicated numerical field identifier in message "
                              << "'" << cluon::trim(prefix) << "': " << duplicatedFieldIdentifier << '\n';
                }
            }
            // Try finding duplicated field names.
            if (retVal) {
                std::sort(std::begin(fieldNames), std::end(fieldNames));
                std::string duplicatedFieldName;
                for (auto it{std::begin(fieldNames)}; it != std::end(fieldNames); it++) {
                    if (it + 1 != std::end(fieldNames)) {
                        if (std::find(it + 1, std::end(fieldNames), *it) != std::end(fieldNames)) {
                            duplicatedFieldName = *it;
                        }
                    }
                }
                retVal &= (duplicatedFieldName.empty());
                if (!retVal) {
                    std::cerr << "[cluon::MessageParser] Found duplicated field name in message '"
                              << cluon::trim(prefix) << "': '" << duplicatedFieldName << "'" << '\n';
                }
            }
        }
        // Within AST node MESSAGE_DECLARATION, we have PRIMITIVE_FIELD from
        // which we need to extract the field "token".
        if (ast.name == "PRIMITIVE_FIELD") {
            // Extract the value of entry "IDENTIFIER".
            auto nodeIdentifier = std::find_if(
                std::begin(ast.nodes), std::end(ast.nodes), [](auto a) { return (a->name == "IDENTIFIER"); });
            if (nodeIdentifier != std::end(ast.nodes)) {
                fieldNames.push_back((*nodeIdentifier)->token);
            }

            // Visit this node's children to check for duplicated numerical identifiers.
            for (const auto &node : ast.nodes) {
                retVal &= checkForUniqueFieldNames(
                    *node, prefix, messageNames, fieldNames, numericalMessageIdentifiers, numericalFieldIdentifiers);
            }
        }
        // Within AST node PRIMITIVE_FIELD, we have PRIMITIVE_FIELD_OPTIONS from
        // which we need to extract the field "token".
        if (ast.name == "PRIMITIVE_FIELD_OPTIONS") {
            // Extract the value of entry "IDENTIFIER".
            auto nodeNumericalFieldIdentifier = std::find_if(
                std::begin(ast.nodes), std::end(ast.nodes), [](auto a) { return (a->name == "NATURAL_NUMBER"); });
            if (nodeNumericalFieldIdentifier != std::end(ast.nodes)) {
                numericalFieldIdentifiers.push_back(std::stoi((*nodeNumericalFieldIdentifier)->token));
            }
        }

        return retVal;
    };

    ////////////////////////////////////////////////////////////////////////////

    // Function to transform AST into list of MetaMessages.
    std::function<void(const peg::Ast &, std::vector<MetaMessage> &)> transform2MetaMessages =
        [](const peg::Ast &ast, std::vector<MetaMessage> &listOfMetaMessages) {
            // "Inner"-lambda to handle various types of message declarations.
            auto createMetaMessage = [](const peg::Ast &_node, std::string _packageName) -> MetaMessage {
                MetaMessage mm;
                mm.packageName(trim(_packageName));
                uint32_t fieldIdentifierCounter{0};
                for (const auto &e : _node.nodes) {
                    if ("MESSAGE_IDENTIFIER" == e->name) {
                        std::string _messageName = e->token;
                        mm.messageName(trim(_messageName));
                    } else if ("NATURAL_NUMBER" == e->name) {
                        mm.messageIdentifier(static_cast<uint32_t>(std::stoi(e->token)));
                    } else if ("PRIMITIVE_FIELD" == e->name) {
                        std::string _fieldName;
                        auto fieldName = std::find_if(
                            std::begin(e->nodes), std::end(e->nodes), [](auto a) { return (a->name == "IDENTIFIER"); });
                        if (fieldName != std::end(e->nodes)) {
                            _fieldName = (*fieldName)->token;
                        }

                        std::string _fieldDataType;
                        auto fieldDataType = std::find_if(std::begin(e->nodes), std::end(e->nodes), [](auto a) {
                            return (a->name == "PRIMITIVE_TYPE");
                        });
                        if (fieldDataType != std::end(e->nodes)) {
                            _fieldDataType = (*fieldDataType)->token;
                        }

                        fieldIdentifierCounter++; // Automatically count expected field identifiers in case of missing
                                                  // field options.
                        std::string _fieldIdentifier;
                        auto fieldIdentifier = std::find_if(std::begin(e->nodes), std::end(e->nodes), [](auto a) {
                            return (a->name == "NATURAL_NUMBER");
                        });
                        if (fieldIdentifier != std::end(e->nodes)) {
                            _fieldIdentifier = (*fieldIdentifier)->token;
                        }

                        std::string _fieldDefaultInitializerValue;
                        auto primitiveFieldOptions = std::find_if(std::begin(e->nodes), std::end(e->nodes), [](auto a) {
                            return (a->name == "PRIMITIVE_FIELD_OPTIONS");
                        });
                        if (primitiveFieldOptions != std::end(e->nodes)) {
                            for (const auto &f : (*primitiveFieldOptions)->nodes) {
                                if ("NATURAL_NUMBER" != f->name) {
                                    if ("STRING" == f->name) {
                                        _fieldDefaultInitializerValue = "\"" + f->token + "\""; // NOLINT
                                    } else if ("CHARACTER" == f->name) {
                                        _fieldDefaultInitializerValue = "'" + f->token + "'";
                                    } else {
                                        _fieldDefaultInitializerValue = f->token;
                                    }
                                }
                            }
                        }

                        std::map<std::string, MetaMessage::MetaField::MetaFieldDataTypes> STRING_TO_DATATYPE_MAP = {
                            {"bool", MetaMessage::MetaField::BOOL_T},
                            {"char", MetaMessage::MetaField::CHAR_T},
                            {"uint8", MetaMessage::MetaField::UINT8_T},
                            {"int8", MetaMessage::MetaField::INT8_T},
                            {"uint16", MetaMessage::MetaField::UINT16_T},
                            {"int16", MetaMessage::MetaField::INT16_T},
                            {"uint32", MetaMessage::MetaField::UINT32_T},
                            {"int32", MetaMessage::MetaField::INT32_T},
                            {"uint64", MetaMessage::MetaField::UINT64_T},
                            {"int64", MetaMessage::MetaField::INT64_T},
                            {"float", MetaMessage::MetaField::FLOAT_T},
                            {"double", MetaMessage::MetaField::DOUBLE_T},
                            {"string", MetaMessage::MetaField::STRING_T},
                            {"bytes", MetaMessage::MetaField::BYTES_T},
                        };

                        MetaMessage::MetaField mf;
                        if (0 < STRING_TO_DATATYPE_MAP.count(_fieldDataType)) {
                            mf.fieldDataType(STRING_TO_DATATYPE_MAP[_fieldDataType]);
                        } else {
                            mf.fieldDataType(MetaMessage::MetaField::MESSAGE_T);
                        }
                        mf.fieldDataTypeName(cluon::trim(_fieldDataType));
                        mf.fieldName(trim(_fieldName));
                        mf.fieldIdentifier((!_fieldIdentifier.empty()
                                                ? static_cast<uint32_t>(std::stoi(trim(_fieldIdentifier)))
                                                : fieldIdentifierCounter));
                        mf.defaultInitializationValue(_fieldDefaultInitializerValue);
                        mm.add(std::move(mf));
                    }
                }
                return mm;
            };

            ////////////////////////////////////////////////////////////////////////

            // Case: "package XYZ" present.
            if ("MESSAGES_SPECIFICATION" == ast.name) {
                // Extract the value of entry "PACKAGE_IDENTIFIER".
                auto nodeIdentifier = std::find_if(std::begin(ast.nodes), std::end(ast.nodes), [](auto a) {
                    return (a->name == "PACKAGE_IDENTIFIER");
                });
                std::string packageName;
                if (nodeIdentifier != std::end(ast.nodes)) {
                    packageName = (*nodeIdentifier)->token;
                }

                // Extract the value of entry "MESSAGE_DECLARATION".
                for (const auto &node : ast.nodes) {
                    if (node->name == "MESSAGE_DECLARATION") {
                        listOfMetaMessages.emplace_back(createMetaMessage(*node, packageName));
                    }
                }
            } else {
                // In case we only have one single message and no package.
                listOfMetaMessages.emplace_back(createMetaMessage(ast, ""));
            }
        };

    ////////////////////////////////////////////////////////////////////////////

    peg::parser p(grammarMessageSpecificationLanguage);
    p.enable_ast();
    p.log = [](size_t row, size_t col, const std::string &msg) {
        std::cerr << "[cluon::MessageParser] Parsing error:" << row << ":" << col << ": " << msg << '\n';
    };

    std::pair<std::vector<MetaMessage>, MessageParserErrorCodes> retVal{};
    std::string inputWithoutComments{input};
    try {
        const std::string MATCH_COMMENTS_REGEX = R"((//.*)|/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)";
        inputWithoutComments = std::regex_replace(input, std::regex(MATCH_COMMENTS_REGEX), ""); // NOLINT
    } catch (std::regex_error &) {                                                              // LCOV_EXCL_LINE
    } catch (std::bad_cast &) {                                                                 // LCOV_EXCL_LINE
    }
    try {
        std::vector<MetaMessage> listOfMetaMessages{};
        std::shared_ptr<peg::Ast> ast{};
        if (p.parse(inputWithoutComments.c_str(), ast)) {
            ast = peg::AstOptimizer(true).optimize(ast);
            {
                std::string tmpPrefix;
                std::vector<std::string> tmpMessageNames{};
                std::vector<std::string> tmpFieldNames{};
                std::vector<int32_t> tmpNumericalMessageIdentifiers{};
                std::vector<int32_t> tmpNumericalFieldIdentifiers{};
                if (check4UniqueFieldNames(*ast,
                                           tmpPrefix,
                                           tmpMessageNames,
                                           tmpFieldNames,
                                           tmpNumericalMessageIdentifiers,
                                           tmpNumericalFieldIdentifiers)) {
                    transform2MetaMessages(*ast, listOfMetaMessages);
                    retVal = {listOfMetaMessages, MessageParserErrorCodes::NO_ERROR};
                } else {
                    retVal = {listOfMetaMessages, MessageParserErrorCodes::DUPLICATE_IDENTIFIERS};
                }
            }
        } else {
            retVal = {listOfMetaMessages, MessageParserErrorCodes::SYNTAX_ERROR};
        }
    } catch (std::bad_cast &) { // LCOV_EXCL_LINE
    }
    return retVal;
}
} // namespace cluon
