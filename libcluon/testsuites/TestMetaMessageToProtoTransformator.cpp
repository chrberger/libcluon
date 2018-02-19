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
#include "cluon/MetaMessageToProtoTransformator.hpp"
#include <iostream>
#include <string>

TEST_CASE("Transforming empty message specification.") {
    const char *input = R"(
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(retVal.first.empty());
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
}

TEST_CASE("Transforming broken message specification.") {
    const char *input = R"(
Hello World
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(retVal.first.empty());
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == retVal.second);
}

TEST_CASE("Transforming one message without package and one field.") {
    const char *input = R"(
message MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_PROTO = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyMessage1 {
    
    optional string s = 1;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO));
    // std::cout << t.content(true) << std::endl;
}

TEST_CASE("Transforming two messages without package and one field.") {
    const char *input = R"(
message MyMessage1 [id = 1] {
    string s [id = 1];
}

message MyMessage2 [id = 2] {
    string s [id = 1];
}
)";

    const char *EXPECTED_PROTO1 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyMessage1 {
    
    optional string s = 1;
    
}
)";

    const char *EXPECTED_PROTO2 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//



// Message identifier: 2.
message MyMessage2 {
    
    optional string s = 1;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());
    {
        auto firstMetaMessage = retVal.first.front();
        cluon::MetaMessageToProtoTransformator t;
        firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        REQUIRE(t.content(true) == std::string(EXPECTED_PROTO1));
        // std::cout << t.content(true) << std::endl;
    }
    {
        auto secondMetaMessage = retVal.first.back();
        cluon::MetaMessageToProtoTransformator t;
        secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        REQUIRE(t.content(false) == std::string(EXPECTED_PROTO2));
        // std::cout << t.content(false) << std::endl;
    }
}

TEST_CASE("Transforming two messages with message prefix, no package, and one field.") {
    const char *input = R"(
message MyPackage.MyMessage1 [id = 1] {
    string s [id = 1];
}

message MyPackage.MyMessage2 [id = 2] {
    string s [id = 1];
}
)";

    const char *EXPECTED_PROTO1 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyPackage_MyMessage1 {
    
    optional string s = 1;
    
}
)";

    const char *EXPECTED_PROTO2 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//



// Message identifier: 2.
message MyPackage_MyMessage2 {
    
    optional string s = 1;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());
    {
        auto firstMetaMessage = retVal.first.front();
        cluon::MetaMessageToProtoTransformator t;
        firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        REQUIRE(t.content(true) == std::string(EXPECTED_PROTO1));
        // std::cout << t.content(true) << std::endl;
    }
    {
        auto secondMetaMessage = retVal.first.back();
        cluon::MetaMessageToProtoTransformator t;
        secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        REQUIRE(t.content(false) == std::string(EXPECTED_PROTO2));
        // std::cout << t.content(false) << std::endl;
    }
}

TEST_CASE("Transforming two messages with package and sub.package and one field.") {
    const char *input = R"(
package MyPackage.SubPackage;
message MyMessage1 [id = 1] {
    string s [id = 1];
}

message MyMessage2 [id = 2] {
    string s [id = 1];
}
)";

    const char *EXPECTED_PROTO1 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyPackage_SubPackage_MyMessage1 {
    
    optional string s = 1;
    
}
)";

    const char *EXPECTED_PROTO2 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//



// Message identifier: 2.
message MyPackage_SubPackage_MyMessage2 {
    
    optional string s = 1;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());
    {
        auto firstMetaMessage = retVal.first.front();
        cluon::MetaMessageToProtoTransformator t;
        firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        REQUIRE(t.content(true) == std::string(EXPECTED_PROTO1));
        // std::cout << t.content(true) << std::endl;
    }
    {
        auto secondMetaMessage = retVal.first.back();
        cluon::MetaMessageToProtoTransformator t;
        secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        REQUIRE(t.content(false) == std::string(EXPECTED_PROTO2));
        // std::cout << t.content(false) << std::endl;
    }
}

TEST_CASE("Transforming two messages with package and one field.") {
    const char *input = R"(
package MyPackage;
message MyMessage1 [id = 1] {
    string s [id = 1];
}

message MyMessage2 [id = 2] {
    string s [id = 1];
}
)";

    const char *EXPECTED_PROTO1 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyPackage_MyMessage1 {
    
    optional string s = 1;
    
}
)";

    const char *EXPECTED_PROTO2 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//



// Message identifier: 2.
message MyPackage_MyMessage2 {
    
    optional string s = 1;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());
    {
        auto firstMetaMessage = retVal.first.front();
        cluon::MetaMessageToProtoTransformator t;
        firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        REQUIRE(t.content(true) == std::string(EXPECTED_PROTO1));
        // std::cout << t.content(true) << std::endl;
    }
    {
        auto secondMetaMessage = retVal.first.back();
        cluon::MetaMessageToProtoTransformator t;
        secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        REQUIRE(t.content(false) == std::string(EXPECTED_PROTO2));
        // std::cout << t.content(false) << std::endl;
    }
}

TEST_CASE("Transforming one message with package and one field.") {
    const char *input = R"(
package MyPackage1;
message MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_PROTO = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyPackage1_MyMessage1 {
    
    optional string s = 1;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO));
    // std::cout << t.content(true) << std::endl;
}

TEST_CASE("Transforming one message with multiple packages and one field.") {
    const char *input = R"(
package MyPackage1.MySubPackage;
message MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_PROTO = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyPackage1_MySubPackage_MyMessage1 {
    
    optional string s = 1;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO));
    // std::cout << t.content(true) << std::endl;
}

TEST_CASE("Transforming one message with sub-package with package and one field.") {
    const char *input = R"(
package MyPackage1;
message MySubPackageMessage.MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_PROTO = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyPackage1_MySubPackageMessage_MyMessage1 {
    
    optional string s = 1;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO));
    // std::cout << t.content(true) << std::endl;
}

TEST_CASE("Transforming one message with sub-package with multiple packages "
          "and one field.") {
    const char *input = R"(
package MyPackage1.MySubPackage;
message MySubPackageMessage.MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_PROTO = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyPackage1_MySubPackage_MySubPackageMessage_MyMessage1 {
    
    optional string s = 1;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO));
    // std::cout << t.content(true) << std::endl;
}

TEST_CASE("Transforming one message and all fields.") {
    const char *input          = R"(
message MyMessage1 [id = 1] {
    bool attribute1 [id = 1];
    char attribute2 [id = 2];
    int32 attribute3 [id = 3];
    uint32 attribute4 [id = 4];
    float attribute5 [id = 5];
    double attribute6 [id = 6];
    string attribute7 [id = 7];
}
)";
    const char *EXPECTED_PROTO = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyMessage1 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
    optional uint32 attribute4 = 4;
    
    optional float attribute5 = 5;
    
    optional double attribute6 = 6;
    
    optional string attribute7 = 7;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO));
    // std::cout << t.content(true) << std::endl;
}

TEST_CASE("Transforming one message and all fields and individual initializers.") {
    const char *input          = R"(
message MyMessage1 [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
    uint8 attribute4 [default = 2, id = 4];
    int16 attribute5 [default = -3, id = 5];
    uint16 attribute6 [default = 4, id = 6];
    int32 attribute7 [default = -5, id = 7];
    uint32 attribute8 [default = 6, id = 8];
    int64 attribute9 [default = -7, id = 9];
    uint64 attribute10 [default = 8, id = 10];
    float attribute11 [default = -9.5, id = 11];
    double attribute12 [default = 10.6, id = 12];
    string attribute13 [default = "Hello World", id = 13];
    bytes attribute14 [default = "Hello Galaxy", id = 14];
}
)";
    const char *EXPECTED_PROTO = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyMessage1 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
    optional uint32 attribute4 = 4;
    
    optional sint32 attribute5 = 5;
    
    optional uint32 attribute6 = 6;
    
    optional sint32 attribute7 = 7;
    
    optional uint32 attribute8 = 8;
    
    optional sint64 attribute9 = 9;
    
    optional uint64 attribute10 = 10;
    
    optional float attribute11 = 11;
    
    optional double attribute12 = 12;
    
    optional string attribute13 = 13;
    
    optional bytes attribute14 = 14;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO));
    // std::cout << t.content(true) << std::endl;
}

TEST_CASE("Transforming two messages with cross-referenced type.") {
    const char *input            = R"(
message MyMessage1 [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
}

message MyMessage2 [id = 2] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
    MyMessage1 attribute4 [id = 4];
}
)";
    const char *EXPECTED_PROTO_1 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message MyMessage1 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
}
)";
    const char *EXPECTED_PROTO_2 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//



// Message identifier: 2.
message MyMessage2 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
    optional MyMessage1 attribute4 = 4;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first[0];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO_1));
    //    std::cout << t.content(true) << std::endl;

    auto secondMetaMessage = retVal.first[1];
    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(false) == std::string(EXPECTED_PROTO_2));
    //    std::cout << t.content(false) << std::endl;
}

TEST_CASE("Transforming two messages with cross-referenced type and package.") {
    const char *input            = R"(
package ABC;
message MyMessage1 [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
}

message MyMessage2 [id = 2] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
    MyMessage1 attribute4 [id = 4];
}
)";
    const char *EXPECTED_PROTO_1 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message ABC_MyMessage1 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
}
)";
    const char *EXPECTED_PROTO_2 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//



// Message identifier: 2.
message ABC_MyMessage2 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
    optional ABC_MyMessage1 attribute4 = 4;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first[0];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO_1));
    //    std::cout << t.content(true) << std::endl;

    auto secondMetaMessage = retVal.first[1];
    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(false) == std::string(EXPECTED_PROTO_2));
    //    std::cout << t.content(false) << std::endl;
}

TEST_CASE("Transforming two messages with cross-referenced type and sub message name.") {
    const char *input            = R"(
message DEF.MyMessage1 [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
}

message DEF.MyMessage2 [id = 2] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
    DEF.MyMessage1 attribute4 [id = 4];
}
)";
    const char *EXPECTED_PROTO_1 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message DEF_MyMessage1 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
}
)";
    const char *EXPECTED_PROTO_2 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//



// Message identifier: 2.
message DEF_MyMessage2 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
    optional DEF_MyMessage1 attribute4 = 4;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first[0];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO_1));
    //    std::cout << t.content(true) << std::endl;

    auto secondMetaMessage = retVal.first[1];
    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(false) == std::string(EXPECTED_PROTO_2));
    //    std::cout << t.content(false) << std::endl;
}

TEST_CASE("Transforming two messages with cross-referenced type and sub message name and package name.") {
    const char *input            = R"(
package ABC;
message DEF.MyMessage1 [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
}

message DEF.MyMessage2 [id = 2] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
    DEF.MyMessage1 attribute4 [id = 4];
}
)";
    const char *EXPECTED_PROTO_1 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message ABC_DEF_MyMessage1 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
}
)";
    const char *EXPECTED_PROTO_2 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//



// Message identifier: 2.
message ABC_DEF_MyMessage2 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
    optional ABC_DEF_MyMessage1 attribute4 = 4;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first[0];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO_1));
    //    std::cout << t.content(true) << std::endl;

    auto secondMetaMessage = retVal.first[1];
    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(false) == std::string(EXPECTED_PROTO_2));
    //    std::cout << t.content(false) << std::endl;
}

TEST_CASE("Transforming two messages with cross-referenced type and sub message name and package name with sub-package.") {
    const char *input            = R"(
package ABC.GHI;
message DEF.MyMessage1 [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
}

message DEF.MyMessage2 [id = 2] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
    DEF.MyMessage1 attribute4 [id = 4];
}
)";
    const char *EXPECTED_PROTO_1 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 1.
message ABC_GHI_DEF_MyMessage1 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
}
)";
    const char *EXPECTED_PROTO_2 = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//



// Message identifier: 2.
message ABC_GHI_DEF_MyMessage2 {
    
    optional bool attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional sint32 attribute3 = 3;
    
    optional ABC_GHI_DEF_MyMessage1 attribute4 = 4;
    
}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());

    cluon::MetaMessageToProtoTransformator t;
    auto firstMetaMessage = retVal.first[0];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(true) == std::string(EXPECTED_PROTO_1));
    //    std::cout << t.content(true) << std::endl;

    auto secondMetaMessage = retVal.first[1];
    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    REQUIRE(t.content(false) == std::string(EXPECTED_PROTO_2));
    //    std::cout << t.content(false) << std::endl;
}
