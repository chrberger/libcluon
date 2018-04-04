/*
 * Copyright (C) 2018  Christian Berger
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

#include "cluon/FromJSONVisitor.hpp"
#include "cluon/ToJSONVisitor.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <sstream>
#include <string>

TEST_CASE("Testing TimeStamp-1.") {
    cluon::data::TimeStamp ts1;
    REQUIRE(ts1.seconds() == 0);
    REQUIRE(ts1.microseconds() == 0);

    ts1.seconds(123).microseconds(456);

    cluon::ToJSONVisitor jsonEncoder;
    ts1.accept(jsonEncoder);

    const char *JSON = R"({"seconds":123,
"microseconds":456})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    cluon::data::TimeStamp ts2;
    REQUIRE(ts2.seconds() == 0);
    REQUIRE(ts2.microseconds() == 0);

    ts2.accept(jsonDecoder);

    REQUIRE(ts2.seconds() == ts1.seconds());
    REQUIRE(ts2.microseconds() == ts1.microseconds());
}

TEST_CASE("Testing TimeStamp-2.") {
    cluon::data::TimeStamp ts1;
    REQUIRE(ts1.seconds() == 0);
    REQUIRE(ts1.microseconds() == 0);

    ts1.seconds(123).microseconds(121);

    cluon::ToJSONVisitor jsonEncoder;
    ts1.accept(jsonEncoder);

    const char *JSON = R"({"seconds":123,
"microseconds":121})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    cluon::data::TimeStamp ts2;
    REQUIRE(ts2.seconds() == 0);
    REQUIRE(ts2.microseconds() == 0);

    ts2.accept(jsonDecoder);

    REQUIRE(ts2.seconds() == ts1.seconds());
    REQUIRE(ts2.microseconds() == ts1.microseconds());
}

TEST_CASE("Testing TimeStamp-3.") {
    cluon::data::TimeStamp ts1;
    REQUIRE(ts1.seconds() == 0);
    REQUIRE(ts1.microseconds() == 0);

    ts1.seconds(123).microseconds(65321);

    cluon::ToJSONVisitor jsonEncoder;
    ts1.accept(jsonEncoder);

    const char *JSON = R"({"seconds":123,
"microseconds":65321})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    cluon::data::TimeStamp ts2;
    REQUIRE(ts2.seconds() == 0);
    REQUIRE(ts2.microseconds() == 0);

    ts2.accept(jsonDecoder);

    REQUIRE(ts2.seconds() == ts1.seconds());
    REQUIRE(ts2.microseconds() == ts1.microseconds());
}

#ifndef WIN32
TEST_CASE("Testing MyTestMessage0.") {
    testdata::MyTestMessage0 tmp;
    REQUIRE(tmp.attribute1());
    REQUIRE('c' == tmp.attribute2());

    tmp.attribute2('d');

    cluon::ToJSONVisitor jsonEncoder;
    tmp.accept(jsonEncoder);

    const char *JSON = R"({"attribute1":1,
"attribute2":"d"})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    testdata::MyTestMessage0 tmp2;
    REQUIRE(tmp2.attribute1());
    REQUIRE('c' == tmp2.attribute2());

    tmp2.accept(jsonDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
    REQUIRE(tmp2.attribute2() == tmp.attribute2());
}

TEST_CASE("Testing MyTestMessage0 with false value.") {
    testdata::MyTestMessage0 tmp;
    REQUIRE(tmp.attribute1());
    REQUIRE('c' == tmp.attribute2());

    tmp.attribute1(false).attribute2('d');

    cluon::ToJSONVisitor jsonEncoder;
    tmp.accept(jsonEncoder);

    const char *JSON = R"({"attribute1":0,
"attribute2":"d"})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    testdata::MyTestMessage0 tmp2;
    REQUIRE(tmp2.attribute1());
    REQUIRE('c' == tmp2.attribute2());

    tmp2.accept(jsonDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
    REQUIRE(tmp2.attribute2() == tmp.attribute2());
}

TEST_CASE("Testing MyTestMessage9.") {
    testdata::MyTestMessage9 tmp;

    tmp.attribute1(1.1f);
    tmp.attribute2(1.23456789);
    REQUIRE(1.1f == Approx(tmp.attribute1()));
    REQUIRE(1.23456789 == Approx(tmp.attribute2()));

    cluon::ToJSONVisitor jsonEncoder;
    tmp.accept(jsonEncoder);

    const char *JSON = R"({"attribute1":1.1,
"attribute2":1.23456789})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    testdata::MyTestMessage9 tmp2;
    tmp2.accept(jsonDecoder);

    REQUIRE(1.1f == Approx(tmp2.attribute1()));
    REQUIRE(1.23456789 == Approx(tmp2.attribute2()));
}

TEST_CASE("Testing MyTestMessage3.") {
    testdata::MyTestMessage3 tmp;
    REQUIRE(124 == tmp.attribute1());
    REQUIRE(-124 == tmp.attribute2());

    tmp.attribute1(123).attribute2(-123);
    REQUIRE(123 == tmp.attribute1());
    REQUIRE(-123 == tmp.attribute2());

    cluon::ToJSONVisitor jsonEncoder;
    tmp.accept(jsonEncoder);

    const char *JSON = R"({"attribute1":123,
"attribute2":-123})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    testdata::MyTestMessage3 tmp2;
    REQUIRE(124 == tmp2.attribute1());
    REQUIRE(-124 == tmp2.attribute2());

    tmp2.accept(jsonDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
    REQUIRE(tmp2.attribute2() == tmp.attribute2());
}

TEST_CASE("Testing MyTestMessage4.") {
    testdata::MyTestMessage4 tmp;
    REQUIRE(tmp.attribute1().empty());

    tmp.attribute1("testing");
    REQUIRE("testing" == tmp.attribute1());

    cluon::ToJSONVisitor jsonEncoder;
    tmp.accept(jsonEncoder);

    const char *JSON = R"({"attribute1":"dGVzdGluZw=="})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    testdata::MyTestMessage4 tmp2;
    REQUIRE(tmp2.attribute1().empty());

    tmp2.accept(jsonDecoder);

    REQUIRE("testing" == tmp2.attribute1());
}

TEST_CASE("Testing MyTestMessage5.") {
    testdata::MyTestMessage5 tmp;

    REQUIRE(1 == tmp.attribute1());
    REQUIRE(-1 == tmp.attribute2());
    REQUIRE(100 == tmp.attribute3());
    REQUIRE(-100 == tmp.attribute4());
    REQUIRE(10000 == tmp.attribute5());
    REQUIRE(-10000 == tmp.attribute6());
    REQUIRE(12345 == tmp.attribute7());
    REQUIRE(-12345 == tmp.attribute8());
    REQUIRE(-1.2345f == Approx(tmp.attribute9()));
    REQUIRE(-10.2345 == Approx(tmp.attribute10()));
    REQUIRE("Hello World!" == tmp.attribute11());

    tmp.attribute1(3)
        .attribute2(-3)
        .attribute3(103)
        .attribute4(-103)
        .attribute5(10003)
        .attribute6(-10003)
        .attribute7(54321)
        .attribute8(-74321)
        .attribute9(-5.4321f)
        .attribute10(-50.4321)
        .attribute11("Hello cluon World!");

    REQUIRE(3 == tmp.attribute1());
    REQUIRE(-3 == tmp.attribute2());
    REQUIRE(103 == tmp.attribute3());
    REQUIRE(-103 == tmp.attribute4());
    REQUIRE(10003 == tmp.attribute5());
    REQUIRE(-10003 == tmp.attribute6());
    REQUIRE(54321 == tmp.attribute7());
    REQUIRE(-74321 == tmp.attribute8());
    REQUIRE(-5.4321f == Approx(tmp.attribute9()));
    REQUIRE(-50.4321 == Approx(tmp.attribute10()));
    REQUIRE("Hello cluon World!" == tmp.attribute11());

    cluon::ToJSONVisitor jsonEncoder;
    tmp.accept(jsonEncoder);

    const char *JSON = R"({"attribute1":3,
"attribute2":-3,
"attribute3":103,
"attribute4":-103,
"attribute5":10003,
"attribute6":-10003,
"attribute7":54321,
"attribute8":-74321,
"attribute9":-5.4321,
"attribute10":-50.4321,
"attribute11":"SGVsbG8gY2x1b24gV29ybGQh"})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    testdata::MyTestMessage5 tmp2;
    REQUIRE(1 == tmp2.attribute1());
    REQUIRE(-1 == tmp2.attribute2());
    REQUIRE(100 == tmp2.attribute3());
    REQUIRE(-100 == tmp2.attribute4());
    REQUIRE(10000 == tmp2.attribute5());
    REQUIRE(-10000 == tmp2.attribute6());
    REQUIRE(12345 == tmp2.attribute7());
    REQUIRE(-12345 == tmp2.attribute8());
    REQUIRE(-1.2345f == Approx(tmp2.attribute9()));
    REQUIRE(-10.2345 == Approx(tmp2.attribute10()));
    REQUIRE("Hello World!" == tmp2.attribute11());

    tmp2.accept(jsonDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
    REQUIRE(tmp2.attribute2() == tmp.attribute2());
    REQUIRE(tmp2.attribute3() == tmp.attribute3());
    REQUIRE(tmp2.attribute4() == tmp.attribute4());
    REQUIRE(tmp2.attribute5() == tmp.attribute5());
    REQUIRE(tmp2.attribute6() == tmp.attribute6());
    REQUIRE(tmp2.attribute7() == tmp.attribute7());
    REQUIRE(tmp2.attribute8() == tmp.attribute8());
    REQUIRE(tmp2.attribute9() == Approx(tmp.attribute9()));
    REQUIRE(tmp2.attribute10() == Approx(tmp.attribute10()));
    REQUIRE(tmp2.attribute11() == tmp.attribute11());
}

TEST_CASE("Testing MyTestMessage6 with visitor to visit nested message for serialization and deserialization.") {
    testdata::MyTestMessage6 tmp6;

    REQUIRE(123 == tmp6.attribute1().attribute1());

    testdata::MyTestMessage2 tmp2;
    tmp6.attribute1(tmp2.attribute1(150));

    REQUIRE(150 == tmp6.attribute1().attribute1());

    cluon::ToJSONVisitor jsonEncoder;
    tmp6.accept(jsonEncoder);

    const char *JSON = R"({"attribute1":{"attribute1":150}})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    testdata::MyTestMessage6 tmp6_2;
    REQUIRE(123 == tmp6_2.attribute1().attribute1());
    tmp6_2.accept(jsonDecoder);
    REQUIRE(150 == tmp6_2.attribute1().attribute1());
}

TEST_CASE("Testing MyTestMessage7 with visitor to visit nested messages for serialization and deserialization.") {
    testdata::MyTestMessage7 tmp7;

    REQUIRE(123 == tmp7.attribute1().attribute1());
    REQUIRE(12345 == tmp7.attribute2());
    REQUIRE(123 == tmp7.attribute3().attribute1());

    testdata::MyTestMessage2 tmp2_1;
    tmp7.attribute1(tmp2_1.attribute1(9));

    tmp7.attribute2(12);

    testdata::MyTestMessage2 tmp2_3;
    tmp7.attribute3(tmp2_3.attribute1(13));

    REQUIRE(9 == tmp7.attribute1().attribute1());
    REQUIRE(12 == tmp7.attribute2());
    REQUIRE(13 == tmp7.attribute3().attribute1());

    cluon::ToJSONVisitor jsonEncoder;
    tmp7.accept(jsonEncoder);

    const char *JSON = R"({"attribute1":{"attribute1":9},
"attribute2":12,
"attribute3":{"attribute1":13}})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};

    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    testdata::MyTestMessage7 tmp7_2;
    REQUIRE(123 == tmp7_2.attribute1().attribute1());
    REQUIRE(12345 == tmp7_2.attribute2());
    REQUIRE(123 == tmp7_2.attribute3().attribute1());

    tmp7_2.accept(jsonDecoder);
    REQUIRE(9 == tmp7_2.attribute1().attribute1());
    REQUIRE(12 == tmp7_2.attribute2());
    REQUIRE(13 == tmp7_2.attribute3().attribute1());
}

TEST_CASE("Transform Envelope into JSON represention for simple payload.") {
    cluon::data::Envelope env;
    REQUIRE(env.serializedData().empty());
    REQUIRE(0 == env.senderStamp());
    REQUIRE(0 == env.dataType());
    REQUIRE(0 == env.sent().seconds());
    REQUIRE(0 == env.sent().microseconds());
    REQUIRE(0 == env.received().seconds());
    REQUIRE(0 == env.received().microseconds());
    REQUIRE(0 == env.sampleTimeStamp().seconds());
    REQUIRE(0 == env.sampleTimeStamp().microseconds());

    cluon::data::TimeStamp ts1;
    ts1.seconds(1).microseconds(2);
    cluon::data::TimeStamp ts2;
    ts2.seconds(10).microseconds(20);
    cluon::data::TimeStamp ts3;
    ts3.seconds(100).microseconds(200);

    env.senderStamp(2).sent(ts1).received(ts2).sampleTimeStamp(ts3).dataType(12);
    {
        cluon::data::TimeStamp payload;
        payload.seconds(3).microseconds(4);

        cluon::ToProtoVisitor proto;
        payload.accept(proto);
        env.serializedData(proto.encodedData());
    }

    REQUIRE(2 == env.senderStamp());
    REQUIRE(12 == env.dataType());
    REQUIRE(1 == env.sent().seconds());
    REQUIRE(2 == env.sent().microseconds());
    REQUIRE(10 == env.received().seconds());
    REQUIRE(20 == env.received().microseconds());
    REQUIRE(100 == env.sampleTimeStamp().seconds());
    REQUIRE(200 == env.sampleTimeStamp().microseconds());

    REQUIRE(4 == env.serializedData().size());
    REQUIRE(0x8 == env.serializedData().at(0));
    REQUIRE(0x6 == env.serializedData().at(1));
    REQUIRE(0x10 == env.serializedData().at(2));
    REQUIRE(0x8 == env.serializedData().at(3));

    // Next, turn Envelope into JSON-encoded byte stream.
    cluon::ToJSONVisitor jsonEncoder;
    env.accept(jsonEncoder);

    const char *JSON = R"({"dataType":12,
"serializedData":"CAYQCA==",
"sent":{"seconds":1,
"microseconds":2},
"received":{"seconds":10,
"microseconds":20},
"sampleTimeStamp":{"seconds":100,
"microseconds":200},
"senderStamp":2})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    {
        cluon::data::Envelope env2;
        REQUIRE(env2.serializedData().empty());
        REQUIRE(0 == env2.senderStamp());
        REQUIRE(0 == env2.dataType());
        REQUIRE(0 == env2.sent().seconds());
        REQUIRE(0 == env2.sent().microseconds());
        REQUIRE(0 == env2.received().seconds());
        REQUIRE(0 == env2.received().microseconds());
        REQUIRE(0 == env2.sampleTimeStamp().seconds());
        REQUIRE(0 == env2.sampleTimeStamp().microseconds());

        std::stringstream sstr{jsonEncoder.json()};

        cluon::FromJSONVisitor jsonDecoder;
        jsonDecoder.decodeFrom(sstr);

        env2.accept(jsonDecoder);
        REQUIRE(!env2.serializedData().empty());
        REQUIRE(2 == env2.senderStamp());
        REQUIRE(12 == env2.dataType());
        REQUIRE(1 == env2.sent().seconds());
        REQUIRE(2 == env2.sent().microseconds());
        REQUIRE(10 == env2.received().seconds());
        REQUIRE(20 == env2.received().microseconds());
        REQUIRE(100 == env2.sampleTimeStamp().seconds());
        REQUIRE(200 == env2.sampleTimeStamp().microseconds());

        REQUIRE(4 == env2.serializedData().size());
        REQUIRE(0x8 == env2.serializedData().at(0));
        REQUIRE(0x6 == env2.serializedData().at(1));
        REQUIRE(0x10 == env2.serializedData().at(2));
        REQUIRE(0x8 == env2.serializedData().at(3));
    }
}
#endif
