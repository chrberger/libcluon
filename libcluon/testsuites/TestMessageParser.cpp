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

#include "catch.hpp"

#include "cluon/MessageParser.hpp"
#include "cluon/MetaMessage.hpp"
#include <string>

TEST_CASE("Parsing empty message specification.") {
    cluon::MessageParser mp;

    const char *tmp = R"()";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Broken message specification.") {
    cluon::MessageParser mp;

    const char *tmp = R"(Hello World.)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing package statement.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package MyPackage;
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing package statement with comment after valid statement.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package MyPackage; // Added comment after valid statement.
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing extended package statement.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package MyPackage.withSubPackage;
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken package statement.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package 1MyPackage;
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken extended package statement.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package MyPackage.2withSubPackage;
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing package and message statement with no fields.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package MyPackage.withSubPackage;

message myMessage [id = 1] {}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing package and message statement with no fields and some comments.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package MyPackage.withSubPackage; // Comment 1
// Comment 2
// Comment 3
message myMessage [id = 1] {}
// Comment 4
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with no fields.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage [id = 1] {}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with no fields.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message 1myMessage [id = 1] {}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with no fields.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with no fields and "
          "trailing comma.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1,] {}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name with no fields.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.2SubName [id = 1] {}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name and signed ID "
          "with no fields.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = -1] {}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name and wrong ID "
          "with no fields.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 0] {}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name and broken "
          "options with no fields.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [i = 1] {}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one int8 field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    int8  attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::INT8_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one int8 field "
          "and no field options and some comments.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
// Comment 1
message myMessage.SubName [id = 1] { // Comment 2
    int8  attribute1; // Coment 3
} // Comment 4
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one int8 field "
          "and no field options and some multi-line comments.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
// Comment 1
message myMessage.SubName [id = 1] { // Comment 2
    int8  attribute1; /* Coment 3
More comments
*/
} // Comment 4
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with two int8 fields "
          "and no field options and some multi-line comments.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
// Comment 1
message myMessage.SubName [id = 1] { // Comment 2
    int8  attribute1; /* Coment 3
More comments
*/
/*
 *   float abc;
 *   double def;
 */
    int8  attribute2; /* Coment 3 */
} // Comment 4
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with two int8 fields "
          "and no field options and comment header.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
/*
 * Copyright (C) 2018 Chalmers Revere
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* Block comment */
message myMessage.SubName [id = 1] {
    int8  attribute1; // Test comment
    int8  attribute2; /* Multi line
* Comment
*
*/
}

//message myMessage.SubName [id = 1] { // Comment 2
//    int8  attribute1;
//}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(1 == retVal.first.size());
}

TEST_CASE("Parsing message statement with extended name with one uint8 field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    uint8 attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::UINT8_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one int16 field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    int16  attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::INT16_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one uint16 field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    uint16 attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::UINT16_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one int32 field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    int32  attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::INT32_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one uint32 field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    uint32 attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::UINT32_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one int64 field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    int64  attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::INT64_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one uint64 field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    uint64 attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::UINT64_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one float field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    float attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::FLOAT_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one double field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    double attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::DOUBLE_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one double field "
          "and positive default initializer.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    double attribute1 [default = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one double field "
          "and signed positive default initializer.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    double attribute1 [default = +2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one double field "
          "and signed positive default initializer with floating point.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    double attribute1 [default = +2.];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one double field "
          "and signed negative default initializer.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    double attribute1 [default = -7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one double field "
          "and signed negative default initializer with floating point.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    double attribute1 [default = -7.];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one char field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    char attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::CHAR_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one string field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    string attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::STRING_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one bool field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::BOOL_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing message statement with extended name with one bytes field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bytes attribute1;
}
)";
    auto retVal     = mp.parse(std::string(tmp));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(!retVal.first.empty());
    REQUIRE(!retVal.first.front().listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::BYTES_T == retVal.first.front().listOfMetaFields()[0].fieldDataType());
    REQUIRE("attribute1" == retVal.first.front().listOfMetaFields()[0].fieldName());
}

TEST_CASE("Parsing broken message statement with extended name with one field "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool 2attribute1;
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one field and "
          "empty field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one field and "
          "wrong numerial field identifier.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [id = 0];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with one field and two "
          "digit numerial field identifier.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [id = 10];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with all field types "
          "and no field options.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1;
    char attribute2;
    int8 attribute3a;
    uint8 attribute4a;
    int16 attribute3b;
    uint16 attribute4b;
    int32 attribute3c;
    uint32 attribute4c;
    int64 attribute3d;
    uint64 attribute4d;
    float attribute5;
    double attribute6;
    string attribute7;
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with various field "
          "types and field identifiers.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [id = 1];
    char attribute2 [id = 2];
    int32 attribute3 [id = 3];
    uint32 attribute4 [id = 4];
    float attribute5 [id = 5];
    double attribute6 [id = 6];
    string attribute7 [id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing two message statements with extended name with various "
          "field types and field identifiers.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubNameA [id = 1] {
    bool attribute1 [id = 1];
    char attribute2 [id = 2];
    int32 attribute3 [id = 3];
    uint32 attribute4 [id = 4];
    float attribute5 [id = 5];
    double attribute6 [id = 6];
    string attribute7 [id = 7];
}

message myMessage.SubNameB [id = 2] {
    bool attribute1 [id = 1];
    char attribute2 [id = 2];
    int32 attribute3 [id = 3];
    uint32 attribute4 [id = 4];
    float attribute5 [id = 5];
    double attribute6 [id = 6];
    string attribute7 [id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with various field "
          "types and default field initializers.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [default = true];
    char attribute2 [default = 'c'];
    int32 attribute3 [default = -12];
    uint32 attribute4 [default = +45];
    float attribute5 [default = -0.12];
    double attribute6 [default = +0.45678];
    string attribute7 [default = "Hello World."];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with various field "
          "types and default field initializers and field identifiers.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with duplicated message identifiers in set of two messages.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 17] {
    bool attribute1 [default = true, id = 1];
}

message myMessage2.SubName4 [id = 17] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with duplicated message identifiers in set of three messages (A).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessage2.SubName4 [id = 14] {
    bool attribute2 [default = true, id = 2];
}

message myMessage3.SubName [id = 23] {
    bool attribute1 [default = true, id = 1];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with duplicated message identifiers in set of three messages (B).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessage3.SubName [id = 23] {
    bool attribute1 [default = true, id = 1];
}

message myMessage2.SubName4 [id = 14] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with duplicated message identifiers in set of three messages (C).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 23] {
    bool attribute1 [default = true, id = 1];
}

message myMessage3.SubName [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessage2.SubName4 [id = 14] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name with duplicated "
          "field names at the beginning.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [default = true, id = 1];
    uint64 attribute1 [default = +54, id = 8];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name with duplicated "
          "field names at the end.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
    uint64 attribute4 [default = +54, id = 8];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name with duplicated "
          "numerical field identifier at the beginning.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [default = true, id = 1];
    uint64 attribute8 [default = +54, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name with duplicated "
          "numerical field identifier at the end.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
    uint64 attribute8 [default = +54, id = 5];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with duplicated message identifiers in set of two messages with package name.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package ABC;
message myMessageA [id = 17] {
    bool attribute1 [default = true, id = 1];
}

message myMessageB [id = 17] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with duplicated message identifiers in set of three messages (A) with package name.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package ABC;
message myMessageA [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageB [id = 14] {
    bool attribute2 [default = true, id = 2];
}

message myMessageC [id = 23] {
    bool attribute1 [default = true, id = 1];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with duplicated message identifiers in set of three messages (B) with package name.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package ABC;
message myMessageA [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageB [id = 23] {
    bool attribute1 [default = true, id = 1];
}

message myMessageC [id = 14] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with duplicated message identifiers in set of three messages (C) with package name.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package ABC;
message myMessageA [id = 23] {
    bool attribute1 [default = true, id = 1];
}

message myMessageB [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageC [id = 14] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name with duplicated "
          "field names at the beginning with package name.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package ABC;
message myMessage [id = 1] {
    bool attribute1 [default = true, id = 1];
    uint64 attribute1 [default = +54, id = 8];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name with duplicated "
          "field names at the end with package name.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package ABC;
message myMessage [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
    uint64 attribute4 [default = +54, id = 8];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name with duplicated "
          "numerical field identifier at the beginning with package name.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package ABC;
message myMessage [id = 1] {
    bool attribute1 [default = true, id = 1];
    uint64 attribute8 [default = +54, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken message statement with extended name with duplicated "
          "numerical field identifier at the end with package name.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package ABC;
message myMessage [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
    uint64 attribute8 [default = +54, id = 5];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing simple message statement with duplicated message identifiers in set of two messages.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessageA [id = 17] {
    bool attribute1 [default = true, id = 1];
}

message myMessageB [id = 17] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing simple message statement with duplicated message identifiers in set of three messages (A).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessageA [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageB [id = 14] {
    bool attribute2 [default = true, id = 2];
}

message myMessageC [id = 23] {
    bool attribute1 [default = true, id = 1];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing simple message statement with duplicated message identifiers in set of three messages (B).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessageA [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageB [id = 23] {
    bool attribute1 [default = true, id = 1];
}

message myMessageC [id = 14] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing simple message statement with duplicated message identifiers in set of three messages (C).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessageA [id = 23] {
    bool attribute1 [default = true, id = 1];
}

message myMessageB [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageC [id = 14] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken simple message statement with extended name with duplicated "
          "field names at the beginning.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage [id = 1] {
    bool attribute1 [default = true, id = 1];
    uint64 attribute1 [default = +54, id = 8];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken simple message statement with extended name with duplicated "
          "field names at the end.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
    uint64 attribute4 [default = +54, id = 8];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken simple message statement with extended name with duplicated "
          "numerical field identifier at the beginning.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage [id = 1] {
    bool attribute1 [default = true, id = 1];
    uint64 attribute8 [default = +54, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing broken simple message statement with extended name with duplicated "
          "numerical field identifier at the end.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int32 attribute3 [default = -12, id = 3];
    uint32 attribute4 [default = +45, id = 4];
    float attribute5 [default = -0.12, id = 5];
    double attribute6 [default = +0.45678, id = 6];
    string attribute7 [default = "Hello World.", id = 7];
    uint64 attribute8 [default = +54, id = 5];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing simple message statement with duplicated message names in set of two messages.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessageA [id = 17] {
    bool attribute1 [default = true, id = 1];
}

message myMessageA [id = 18] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing simple message statement with duplicated message names in set of three messages (A).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessageA [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageA [id = 15] {
    bool attribute2 [default = true, id = 2];
}

message myMessageC [id = 23] {
    bool attribute1 [default = true, id = 1];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing simple message statement with duplicated message names in set of three messages (B).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessageA [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageB [id = 23] {
    bool attribute1 [default = true, id = 1];
}

message myMessageA [id = 15] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing simple message statement with duplicated message names in set of three messages (C).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessageA [id = 23] {
    bool attribute1 [default = true, id = 1];
}

message myMessageC [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageC [id = 15] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing simple message statement with duplicated message names in set of three messages (D).") {
    cluon::MessageParser mp;

    const char *tmp = R"(
package ABD;
message myMessageA [id = 23] {
    bool attribute1 [default = true, id = 1];
}

message myMessageC [id = 14] {
    bool attribute1 [default = true, id = 1];
}

message myMessageC [id = 15] {
    bool attribute2 [default = true, id = 2];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::DUPLICATE_IDENTIFIERS == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with string field and "
          "example initializer.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    string attribute7 [default = "ABC", id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with string field and "
          "empty initializer.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    string attribute7 [default = "", id = 7];
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with string field with "
          "field name's size 1.") {
    cluon::MessageParser mp;

    const char *tmp = R"(
message myMessage.SubName [id = 1] {
    string s;
}
)";
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == mp.parse(std::string(tmp)).second);
}

TEST_CASE("Parsing message statement with extended name with string field "
          "into MetaMessage.") {
    cluon::MessageParser mp;

    const char *tmp   = R"(
package This.is.my.complex.Package;
message myMessage.SubName [id = 1] {
    string attribute7 [default = "Hello Galaxy", id = 7];
}
)";
    auto retVal       = mp.parse(std::string(tmp));
    auto metaMessages = retVal.first;
    REQUIRE(!metaMessages.empty());
    REQUIRE("This.is.my.complex.Package" == metaMessages.front().packageName());
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
}

TEST_CASE("Parsing nested messages.") {
    cluon::MessageParser mp;

    const char *tmp   = R"(
message myPackage.MessageA [id = 1] {
    int32 attribute1 [id = 1];
}

message myPackage.MessageB [id = 2] {
    float field1 [id = 1];
    myPackage.MessageA field2 [id = 2];
}
)";
    auto retVal       = mp.parse(std::string(tmp));
    auto metaMessages = retVal.first;
    REQUIRE(!metaMessages.empty());
    REQUIRE(2 == metaMessages.size());
    REQUIRE(metaMessages[0].packageName().empty());
    REQUIRE(metaMessages[1].packageName().empty());
    REQUIRE(cluon::MetaMessage::MetaField::FLOAT_T == metaMessages[1].listOfMetaFields()[0].fieldDataType());
    REQUIRE(cluon::MetaMessage::MetaField::MESSAGE_T == metaMessages[1].listOfMetaFields()[1].fieldDataType());
    REQUIRE("myPackage.MessageA" == metaMessages[0].messageName());
    REQUIRE("myPackage.MessageB" == metaMessages[1].messageName());
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
}
