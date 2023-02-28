/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/MessageParser.hpp"
#include "cluon/stringtoolbox.hpp"

#include "cpp-peglib/peglib.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

namespace cluon {

std::pair<std::vector<MetaMessage>, MessageParser::MessageParserErrorCodes> MessageParser::parse(const std::string &input) {
    const char *grammarMessageSpecificationLanguage = R"(
        MESSAGES_SPECIFICATION      <- PACKAGE_DECLARATION? MESSAGE_DECLARATION*
        PACKAGE_DECLARATION         <- 'package' PACKAGE_NAME ';'
        PACKAGE_NAME                <- < NAME ('.' NAME)* >

        MESSAGE_DECLARATION         <- 'message' MESSAGE_NAME '[' IDENTIFIER ','? ']' '{' FIELD* '}'
        MESSAGE_NAME                <- < NAME ('.' NAME)* >

        FIELD                       <- PRIMITIVE_TYPE NAME ('[' (((DEFAULT / IDENTIFIER) ','?)+)? ']')? ';'
        DEFAULT                     <- 'default' '=' (FLOAT_NUMBER / BOOL / CHARACTER / STRING)
        PRIMITIVE_TYPE              <- < 'bool' / 'float' / 'double' /
                                         'char' /
                                         'bytes' / 'string' /
                                         'int8' / 'uint8' / 
                                         'int16' / 'uint16' / 
                                         'int32' / 'uint32' / 
                                         'int64' / 'uint64' /
                                         MESSAGE_TYPE >

        MESSAGE_TYPE                <- < NAME ('.' NAME)* >

        IDENTIFIER                  <- 'id' '=' NATURAL_NUMBER

        NAME                        <- < [a-zA-Z][a-zA-Z0-9_]* >
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
    std::function<bool(const peg::Ast &, std::string &, std::vector<std::string> &, std::vector<std::string> &, std::vector<int32_t> &, std::vector<int32_t> &)>
        check4UniqueFieldNames = [&checkForUniqueFieldNames = check4UniqueFieldNames](const peg::Ast &ast,
                                                                                      std::string &prefix,
                                                                                      std::vector<std::string> &messageNames,
                                                                                      std::vector<std::string> &fieldNames,
                                                                                      std::vector<int32_t> &numericalMessageIdentifiers,
                                                                                      std::vector<int32_t> &numericalFieldIdentifiers) {
            bool retVal = true;
            // First, we need to visit the children of AST node MESSAGES_SPECIFICATION.

            if ("MESSAGES_SPECIFICATION" == ast.name) {
                for (const auto &node : ast.nodes) {
                    retVal &= checkForUniqueFieldNames(*node, prefix, messageNames, fieldNames, numericalMessageIdentifiers, numericalFieldIdentifiers);
                }
                // Try finding duplicated message identifiers.
                if (retVal) {
                    std::sort(std::begin(numericalMessageIdentifiers), std::end(numericalMessageIdentifiers));
                    int32_t duplicatedMessageIdentifier{-1};
                    for (auto it{std::begin(numericalMessageIdentifiers)}; it != std::end(numericalMessageIdentifiers); it++) {
                        if (it + 1 != std::end(numericalMessageIdentifiers)) {
                            if (std::find(it + 1, std::end(numericalMessageIdentifiers), *it) != std::end(numericalMessageIdentifiers)) {
                                duplicatedMessageIdentifier = *it;
                            }
                        }
                    }
                    retVal &= (-1 == duplicatedMessageIdentifier);
                    if (!retVal) {
                        std::cerr << "[cluon::MessageParser] Found duplicated numerical message identifier: " << duplicatedMessageIdentifier << '\n';
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
                        std::cerr << "[cluon::MessageParser] Found duplicated message name '" << duplicatedMessageName << "'" << '\n';
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
                    if ("MESSAGE_NAME" == node->original_name) {
                        prefix = node->token;
                        messageNames.push_back(::stringtoolbox::trim(prefix));
                    } else if ("IDENTIFIER" == node->original_name) {
                        numericalMessageIdentifiers.push_back(std::stoi(node->token));
                    } else if ("FIELD" == node->original_name) {
                        retVal &= checkForUniqueFieldNames(*node, prefix, messageNames, fieldNames, numericalMessageIdentifiers, numericalFieldIdentifiers);
                    }
                }

                // Try finding duplicated numerical field identifiers.
                if (retVal) {
                    std::sort(std::begin(numericalFieldIdentifiers), std::end(numericalFieldIdentifiers));
                    int32_t duplicatedFieldIdentifier{-1};
                    for (auto it{std::begin(numericalFieldIdentifiers)}; it != std::end(numericalFieldIdentifiers); it++) {
                        if (it + 1 != std::end(numericalFieldIdentifiers)) {
                            if (std::find(it + 1, std::end(numericalFieldIdentifiers), *it) != std::end(numericalFieldIdentifiers)) {
                                duplicatedFieldIdentifier = *it;
                            }
                        }
                    }
                    retVal &= (-1 == duplicatedFieldIdentifier);
                    if (!retVal) {
                        std::cerr << "[cluon::MessageParser] Found duplicated numerical field identifier in message "
                                  << "'" << ::stringtoolbox::trim(prefix) << "': " << duplicatedFieldIdentifier << '\n';
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
                        std::cerr << "[cluon::MessageParser] Found duplicated field name in message '" << ::stringtoolbox::trim(prefix) << "': '"
                                  << duplicatedFieldName << "'" << '\n';
                    }
                }
            }
            // Within AST node MESSAGE_DECLARATION, we have FIELD from
            // which we need to extract the field "token".
            if (ast.original_name == "FIELD") {
                // Extract the value of entry "NAME".
                auto nodeName = std::find_if(std::begin(ast.nodes), std::end(ast.nodes), [](auto a) { return (a->original_name == "NAME"); });
                if (nodeName != std::end(ast.nodes)) {
                    fieldNames.push_back((*nodeName)->token);
                }

                // Extract the value of entry "IDENTIFIER".
                auto nodeNumericalFieldIdentifier
                    = std::find_if(std::begin(ast.nodes), std::end(ast.nodes), [](auto a) { return (a->original_name == "IDENTIFIER"); });
                if (nodeNumericalFieldIdentifier != std::end(ast.nodes)) {
                    numericalFieldIdentifiers.push_back(std::stoi((*nodeNumericalFieldIdentifier)->token));
                }
            }

            return retVal;
        };

    ////////////////////////////////////////////////////////////////////////////

    // Function to transform AST into list of MetaMessages.
    std::function<void(const peg::Ast &, std::vector<MetaMessage> &)> transform2MetaMessages
        = [](const peg::Ast &ast, std::vector<MetaMessage> &listOfMetaMessages) {
              // "Inner"-lambda to handle various types of message declarations.
              auto createMetaMessage = [](const peg::Ast &_node, std::string _packageName) -> MetaMessage {
                  MetaMessage mm;
                  mm.packageName(::stringtoolbox::trim(_packageName));
                  uint32_t fieldIdentifierCounter{0};
                  for (const auto &e : _node.nodes) {
                      if ("MESSAGE_NAME" == e->original_name) {
                          std::string _messageName = e->token;
                          mm.messageName(::stringtoolbox::trim(_messageName));
                      } else if ("IDENTIFIER" == e->original_name) {
                          mm.messageIdentifier(std::stoi(e->token));
                      } else if ("FIELD" == e->original_name) {
                          std::string _fieldDataType;
                          std::string _fieldName;
                          std::string _fieldDefaultInitializerValue;
                          std::string _fieldIdentifier;
                          for (const auto &f : e->nodes) {
                              if ("PRIMITIVE_TYPE" == f->original_name) {
                                  _fieldDataType = f->token;
                              } else if ("NAME" == f->original_name) {
                                  _fieldName = f->token;
                              } else if ("DEFAULT" == f->original_name) {
                                  if ("STRING" == f->name) {
                                      _fieldDefaultInitializerValue = "\"" + f->token + "\""; // NOLINT
                                  } else if ("CHARACTER" == f->name) {
                                      _fieldDefaultInitializerValue = "'" + f->token + "'";
                                  } else {
                                      _fieldDefaultInitializerValue = f->token;
                                  }
                              } else if ("IDENTIFIER" == f->original_name) {
                                  _fieldIdentifier = f->token;
                              }
                          }

                          if (_fieldIdentifier.empty()) {
                              // Automatically count expected field identifiers in case of missing field options.
                              fieldIdentifierCounter++;
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
                          mf.fieldDataTypeName(::stringtoolbox::trim(_fieldDataType));
                          mf.fieldName(::stringtoolbox::trim(_fieldName));
                          mf.fieldIdentifier(
                              (!_fieldIdentifier.empty() ? static_cast<uint32_t>(std::stoi(::stringtoolbox::trim(_fieldIdentifier))) : fieldIdentifierCounter));
                          mf.defaultInitializationValue(_fieldDefaultInitializerValue);
                          mm.add(std::move(mf));
                      }
                  }
                  return mm;
              };

              ////////////////////////////////////////////////////////////////////////

              // Case: "package XYZ" present.
              if ("MESSAGES_SPECIFICATION" == ast.name) {
                  // Extract the value of entry "PACKAGE_NAME".
                  auto nodeIdentifier = std::find_if(std::begin(ast.nodes), std::end(ast.nodes), [](auto a) { return (a->name == "PACKAGE_NAME"); });
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
        const std::string MATCH_COMMENTS_REGEX = R"(/\*([\s\S]*?)\*/|//.*)";
        inputWithoutComments                   = std::regex_replace(input, std::regex(MATCH_COMMENTS_REGEX), ""); // NOLINT
    } catch (std::regex_error &) {                                                                                // LCOV_EXCL_LINE
    } catch (std::bad_cast &) {                                                                                   // LCOV_EXCL_LINE
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
                if (check4UniqueFieldNames(*ast, tmpPrefix, tmpMessageNames, tmpFieldNames, tmpNumericalMessageIdentifiers, tmpNumericalFieldIdentifiers)) {
                    transform2MetaMessages(*ast, listOfMetaMessages);
                    retVal = {listOfMetaMessages, MessageParserErrorCodes::NO_MESSAGEPARSER_ERROR};
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
