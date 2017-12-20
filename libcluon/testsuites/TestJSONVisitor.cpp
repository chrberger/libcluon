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

#include "catch.hpp"
#include <string>

#include "cluon/JSONVisitor.hpp"
#include "cluon/GenericMessage.hpp"
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

    cluon::JSONVisitor j;
    tmp.accept(j);

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
"attribute13":"Hello World",
"attribute14":"Hello Galaxy"})";

    REQUIRE(std::string(JSON) == j.json());
}

TEST_CASE("Testing MyTestMessage6.") {
    testdata::MyTestMessage6 tmp6;
    testdata::MyTestMessage2 tmp2;
    tmp2.attribute1(97);
    tmp6.attribute1(tmp2);

    REQUIRE(97 == tmp6.attribute1().attribute1());

    cluon::JSONVisitor j;
    tmp6.accept(j);

    const char *JSON = R"({"attribute1":{"attribute1":97}})";

    REQUIRE(std::string(JSON) == j.json());
}

TEST_CASE("Testing MyTestMessage1 to GenericMessage to JSON.") {
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

    // Create a generic representation from the given message.
    cluon::GenericMessage gm;
    gm.createFrom<testdata::MyTestMessage1>(tmp);

    // Create a JSON representation from the generic message.
    cluon::JSONVisitor j;
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
"attribute13":"Hello World",
"attribute14":"Hello Galaxy"})";

    REQUIRE(std::string(JSON) == j.json());
}

TEST_CASE("Testing MyTestMessage1 to GenericMessage to MyTestMessage1.") {
    testdata::MyTestMessage1 src;
    src.attribute1(false)
        .attribute2('a')
        .attribute3(-10)
        .attribute4(20)
        .attribute5(-30)
        .attribute6(40)
        .attribute7(-50)
        .attribute8(60)
        .attribute9(-70)
        .attribute10(80)
        .attribute11(-90.5)
        .attribute12(100.6)
        .attribute13("World, Hello")
        .attribute14("Galaxy, Hello");

    REQUIRE(!src.attribute1());
    REQUIRE('a' == src.attribute2());
    REQUIRE(-10 == src.attribute3());
    REQUIRE(20 == src.attribute4());
    REQUIRE(-30 == src.attribute5());
    REQUIRE(40 == src.attribute6());
    REQUIRE(-50 == src.attribute7());
    REQUIRE(60 == src.attribute8());
    REQUIRE(-70 == src.attribute9());
    REQUIRE(80 == src.attribute10());
    REQUIRE(-90.5 == Approx(src.attribute11()));
    REQUIRE(100.6 == Approx(src.attribute12()));
    REQUIRE("World, Hello" == src.attribute13());
    REQUIRE("Galaxy, Hello" == src.attribute14());

    testdata::MyTestMessage1 dest;
    REQUIRE('c' == dest.attribute2());
    REQUIRE(-1 == dest.attribute3());
    REQUIRE(2 == dest.attribute4());
    REQUIRE(-3 == dest.attribute5());
    REQUIRE(4 == dest.attribute6());
    REQUIRE(-5 == dest.attribute7());
    REQUIRE(6 == dest.attribute8());
    REQUIRE(-7 == dest.attribute9());
    REQUIRE(8 == dest.attribute10());
    REQUIRE(-9.5 == Approx(dest.attribute11()));
    REQUIRE(10.6 == Approx(dest.attribute12()));
    REQUIRE("Hello World" == dest.attribute13());
    REQUIRE("Hello Galaxy" == dest.attribute14());

    // Create a generic representation from the given message.
    cluon::GenericMessage gm;
    gm.createFrom<testdata::MyTestMessage1>(src);

    dest.accept(gm);
    REQUIRE(!dest.attribute1());
    REQUIRE('a' == dest.attribute2());
    REQUIRE(-10 == dest.attribute3());
    REQUIRE(20 == dest.attribute4());
    REQUIRE(-30 == dest.attribute5());
    REQUIRE(40 == dest.attribute6());
    REQUIRE(-50 == dest.attribute7());
    REQUIRE(60 == dest.attribute8());
    REQUIRE(-70 == dest.attribute9());
    REQUIRE(80 == dest.attribute10());
    REQUIRE(-90.5 == Approx(dest.attribute11()));
    REQUIRE(100.6 == Approx(dest.attribute12()));
    REQUIRE("World, Hello" == dest.attribute13());
    REQUIRE("Galaxy, Hello" == dest.attribute14());
}

TEST_CASE("Testing MyTestMessage6 to GenericMessage to MyTestMessage6.") {
    testdata::MyTestMessage6 tmp6;
    testdata::MyTestMessage2 tmp2;
    tmp2.attribute1(98);
    tmp6.attribute1(tmp2);

    REQUIRE(98 == tmp6.attribute1().attribute1());

    // Create a generic representation from the given message.
    cluon::GenericMessage gm;
    gm.createFrom<testdata::MyTestMessage6>(tmp6);

    testdata::MyTestMessage6 tmp6_2;
    REQUIRE(123 == tmp6_2.attribute1().attribute1());

    tmp6_2.accept(gm);
    REQUIRE(98 == tmp6_2.attribute1().attribute1());
}

TEST_CASE("Testing MyTestMessage6 to GenericMessage to JSON.") {
    testdata::MyTestMessage6 tmp6;
    testdata::MyTestMessage2 tmp2;
    tmp2.attribute1(97);
    tmp6.attribute1(tmp2);

    REQUIRE(97 == tmp6.attribute1().attribute1());

    // Create a generic representation from the given message.
    cluon::GenericMessage gm;
    gm.createFrom<testdata::MyTestMessage6>(tmp6);
std::cout << __LINE__ << std::endl;
    // Create a JSON representation from the generic message.
    cluon::JSONVisitor j;
    gm.accept(j);

    const char *JSON = R"({"attribute1":{"attribute1":97}})";

    REQUIRE(std::string(JSON) == j.json());
}
