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
