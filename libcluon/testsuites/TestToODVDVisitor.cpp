/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"
#include <string>

#include <iostream>

#include "cluon/FromProtoVisitor.hpp"
#include "cluon/GenericMessage.hpp"
#include "cluon/MessageParser.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/ToJSONVisitor.hpp"
#include "cluon/ToODVDVisitor.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/cluonTestDataStructures.hpp"

TEST_CASE("Testing MyTestMessage1.") {
    testdata::MyTestMessage1 tmp;

    tmp.attribute11(-9.123456f).attribute12(10.123456789);

    REQUIRE(tmp.attribute1());
    REQUIRE('c' == tmp.attribute2());
    REQUIRE(-1 == tmp.attribute3());
    REQUIRE(2 == tmp.attribute4());
    REQUIRE(-3 == tmp.attribute5());
    REQUIRE(4 == tmp.attribute6());
    REQUIRE(-5 == tmp.attribute7());
    REQUIRE(6 == tmp.attribute8());
    REQUIRE(-7 == tmp.attribute9());
    REQUIRE(8 == tmp.attribute10());
    REQUIRE(-9.123456 == Approx(tmp.attribute11()));
    REQUIRE(10.123456789 == Approx(tmp.attribute12()));
    REQUIRE("Hello World" == tmp.attribute13());
    REQUIRE("Hello Galaxy" == tmp.attribute14());

    cluon::ToProtoVisitor protoEncoder;
    tmp.accept(protoEncoder);
    const std::string protoEncoded{protoEncoder.encodedData()};

    cluon::ToODVDVisitor odvdVisitor;
    tmp.accept(odvdVisitor);

    const char *msg = R"(message testdata.MyTestMessage1 [ id = 30001 ] {
    bool attribute1 [ default = false, id = 1 ];
    char attribute2 [ default = '0', id = 2 ];
    int8 attribute3 [ default = 0, id = 3 ];
    uint8 attribute4 [ default = 0, id = 4 ];
    int16 attribute5 [ default = 0, id = 5 ];
    uint16 attribute6 [ default = 0, id = 6 ];
    int32 attribute7 [ default = 0, id = 7 ];
    uint32 attribute8 [ default = 0, id = 8 ];
    int64 attribute9 [ default = 0, id = 9 ];
    uint64 attribute10 [ default = 0, id = 10 ];
    float attribute11 [ default = 0.0, id = 11 ];
    double attribute12 [ default = 0.0, id = 12 ];
    string attribute13 [ default = "", id = 13 ];
    string attribute14 [ default = "", id = 14 ];
}
)";

    const std::string generatedMessageSpecification{odvdVisitor.messageSpecification()};
    REQUIRE(std::string(msg) == generatedMessageSpecification);

    cluon::MessageParser mp;
    auto retVal = mp.parse(generatedMessageSpecification);
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_MESSAGEPARSER_ERROR == retVal.second);
    auto listOfMetaMessages = retVal.first;
    REQUIRE(1 == listOfMetaMessages.size());

    cluon::FromProtoVisitor protoDecoder;
    std::stringstream sstr{protoEncoded};
    protoDecoder.decodeFrom(sstr);

    cluon::MetaMessage m = listOfMetaMessages.front();
    cluon::GenericMessage gm;
    gm.createFrom(m, listOfMetaMessages);
    // Set values in GenericMessage from ProtoDecoder.
    gm.accept(protoDecoder);

    cluon::ToJSONVisitor j;
    gm.accept(j);

    const char *JSON = R"({"attribute1":1,
"attribute2":"c",
"attribute3":-1,
"attribute4":2,
"attribute5":-3,
"attribute6":4,
"attribute7":-5,
"attribute8":6,
"attribute9":-7,
"attribute10":8,
"attribute11":-9.123456,
"attribute12":10.123456789,
"attribute13":"SGVsbG8gV29ybGQ=",
"attribute14":"SGVsbG8gR2FsYXh5"})";

    REQUIRE(std::string(JSON) == j.json());
}

TEST_CASE("Testing MyTestMessage6.") {
    testdata::MyTestMessage6 tmp6;
    testdata::MyTestMessage2 tmp2;
    tmp2.attribute1(97);
    tmp6.attribute1(tmp2);

    REQUIRE(97 == tmp6.attribute1().attribute1());

    cluon::ToProtoVisitor protoEncoder;
    tmp6.accept(protoEncoder);
    const std::string protoEncoded{protoEncoder.encodedData()};

    cluon::ToODVDVisitor odvdVisitor;
    tmp6.accept(odvdVisitor);

    const char *msg = R"(message testdata.MyTestMessage2 [ id = 30002 ] {
    uint8 attribute1 [ default = 0, id = 1 ];
}
message testdata.MyTestMessage6 [ id = 30006 ] {
    testdata.MyTestMessage2 attribute1 [ id = 3 ];
}
)";

    const std::string generatedMessageSpecification{odvdVisitor.messageSpecification()};
    REQUIRE(std::string(msg) == generatedMessageSpecification);

    cluon::MessageParser mp;
    auto retVal = mp.parse(generatedMessageSpecification);
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_MESSAGEPARSER_ERROR == retVal.second);
    auto listOfMetaMessages = retVal.first;
    REQUIRE(2 == listOfMetaMessages.size());

    cluon::FromProtoVisitor protoDecoder;
    std::stringstream sstr{protoEncoded};
    protoDecoder.decodeFrom(sstr);

    cluon::MetaMessage m = listOfMetaMessages[1];
    cluon::GenericMessage gm;
    gm.createFrom(m, listOfMetaMessages);
    // Set values in GenericMessage from ProtoDecoder.
    gm.accept(protoDecoder);

    cluon::ToJSONVisitor j;
    gm.accept(j);

    const char *JSON = R"({"attribute1":{"attribute1":97}})";

    REQUIRE(std::string(JSON) == j.json());
}
