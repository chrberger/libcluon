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
#include <string>

#include "cluon/FromProtoVisitor.hpp"
#include "cluon/GenericMessage.hpp"
#include "cluon/MessageParser.hpp"
#include "cluon/ToCSVVisitor.hpp"
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

    cluon::ToCSVVisitor csv;
    tmp.accept(csv);

    const char *CSV
        = R"(attribute1;attribute2;attribute3;attribute4;attribute5;attribute6;attribute7;attribute8;attribute9;attribute10;attribute11;attribute12;attribute13;attribute14;
1;c;-1;2;-3;4;-5;6;-7;8;-9.123456;10.123456789;"Hello World";"Hello Galaxy";
)";

    REQUIRE(std::string(CSV) == csv.csv());
}

TEST_CASE("Testing MyTestMessage1 with ',' delimiter.") {
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

    cluon::ToCSVVisitor csv(',');
    tmp.accept(csv);

    const char *CSV
        = R"(attribute1,attribute2,attribute3,attribute4,attribute5,attribute6,attribute7,attribute8,attribute9,attribute10,attribute11,attribute12,attribute13,attribute14,
1,c,-1,2,-3,4,-5,6,-7,8,-9.123456,10.123456789,"Hello World","Hello Galaxy",
)";

    REQUIRE(std::string(CSV) == csv.csv());
}

TEST_CASE("Testing MyTestMessage1 with ',' delimiter and appending data.") {
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

    cluon::ToCSVVisitor csv(',');

    const char *CSV1
        = R"(attribute1,attribute2,attribute3,attribute4,attribute5,attribute6,attribute7,attribute8,attribute9,attribute10,attribute11,attribute12,attribute13,attribute14,
1,c,-1,2,-3,4,-5,6,-7,8,-9.123456,10.123456789,"Hello World","Hello Galaxy",
)";
    tmp.accept(csv);
    REQUIRE(std::string(CSV1) == csv.csv());

    tmp.attribute4(77);
    const char *CSV2
        = R"(attribute1,attribute2,attribute3,attribute4,attribute5,attribute6,attribute7,attribute8,attribute9,attribute10,attribute11,attribute12,attribute13,attribute14,
1,c,-1,2,-3,4,-5,6,-7,8,-9.123456,10.123456789,"Hello World","Hello Galaxy",
1,c,-1,77,-3,4,-5,6,-7,8,-9.123456,10.123456789,"Hello World","Hello Galaxy",
)";
    tmp.accept(csv);
    REQUIRE(std::string(CSV2) == csv.csv());

    tmp.attribute4(99);
    const char *CSV3
        = R"(attribute1,attribute2,attribute3,attribute4,attribute5,attribute6,attribute7,attribute8,attribute9,attribute10,attribute11,attribute12,attribute13,attribute14,
1,c,-1,2,-3,4,-5,6,-7,8,-9.123456,10.123456789,"Hello World","Hello Galaxy",
1,c,-1,77,-3,4,-5,6,-7,8,-9.123456,10.123456789,"Hello World","Hello Galaxy",
1,c,-1,99,-3,4,-5,6,-7,8,-9.123456,10.123456789,"Hello World","Hello Galaxy",
)";
    tmp.accept(csv);
    REQUIRE(std::string(CSV3) == csv.csv());

    tmp.attribute4(44);
    const char *CSV4
        = R"(attribute1,attribute2,attribute3,attribute4,attribute5,attribute6,attribute7,attribute8,attribute9,attribute10,attribute11,attribute12,attribute13,attribute14,
1,c,-1,44,-3,4,-5,6,-7,8,-9.123456,10.123456789,"Hello World","Hello Galaxy",
)";
    csv.clear();
    tmp.accept(csv);
    REQUIRE(std::string(CSV4) == csv.csv());
}

TEST_CASE("Testing MyTestMessage1 with ',' delimiter and no header.") {
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

    cluon::ToCSVVisitor csv(',', false);
    tmp.accept(csv);

    const char *CSV = R"(1,c,-1,2,-3,4,-5,6,-7,8,-9.123456,10.123456789,"Hello World","Hello Galaxy",
)";

    REQUIRE(std::string(CSV) == csv.csv());
}

TEST_CASE("Testing MyTestMessage6.") {
    testdata::MyTestMessage6 tmp6;
    testdata::MyTestMessage2 tmp2;
    tmp2.attribute1(97);
    tmp6.attribute1(tmp2);

    REQUIRE(97 == tmp6.attribute1().attribute1());

    cluon::ToCSVVisitor csv;
    tmp6.accept(csv);

    const char *CSV = R"(attribute1.attribute1;
97;
)";

    REQUIRE(std::string(CSV) == csv.csv());
}

TEST_CASE("Testing MyTestMessage6 with ',' delimiter.") {
    testdata::MyTestMessage6 tmp6;
    testdata::MyTestMessage2 tmp2;
    tmp2.attribute1(97);
    tmp6.attribute1(tmp2);

    REQUIRE(97 == tmp6.attribute1().attribute1());

    cluon::ToCSVVisitor csv(',');
    tmp6.accept(csv);

    const char *CSV = R"(attribute1.attribute1,
97,
)";

    REQUIRE(std::string(CSV) == csv.csv());
}

TEST_CASE("Testing MyTestMessage6 with ',' delimiter and no header.") {
    testdata::MyTestMessage6 tmp6;
    testdata::MyTestMessage2 tmp2;
    tmp2.attribute1(97);
    tmp6.attribute1(tmp2);

    REQUIRE(97 == tmp6.attribute1().attribute1());

    cluon::ToCSVVisitor csv(',', false);
    tmp6.accept(csv);

    const char *CSV = R"(97,
)";

    REQUIRE(std::string(CSV) == csv.csv());
}

TEST_CASE("Testing MyTestMessage6 with ',' delimiter and no header and appending data.") {
    testdata::MyTestMessage6 tmp6;
    testdata::MyTestMessage2 tmp2;
    tmp2.attribute1(97);
    tmp6.attribute1(tmp2);

    REQUIRE(97 == tmp6.attribute1().attribute1());

    cluon::ToCSVVisitor csv(',', false);

    const char *CSV1 = R"(97,
)";
    tmp6.accept(csv);
    REQUIRE(std::string(CSV1) == csv.csv());

    tmp2.attribute1(98);
    tmp6.attribute1(tmp2);
    const char *CSV2 = R"(97,
98,
)";
    tmp6.accept(csv);
    REQUIRE(std::string(CSV2) == csv.csv());

    tmp2.attribute1(99);
    tmp6.attribute1(tmp2);
    const char *CSV3 = R"(97,
98,
99,
)";
    tmp6.accept(csv);
    REQUIRE(std::string(CSV3) == csv.csv());

    tmp2.attribute1(100);
    tmp6.attribute1(tmp2);
    const char *CSV4 = R"(100,
)";
    csv.clear();
    tmp6.accept(csv);
    REQUIRE(std::string(CSV4) == csv.csv());
}
