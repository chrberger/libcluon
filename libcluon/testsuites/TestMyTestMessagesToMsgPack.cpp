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

#include "cluon/FromMsgPackVisitor.hpp"
#include "cluon/GenericMessage.hpp"
#include "cluon/MessageParser.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/ToMsgPackVisitor.hpp"
#include "cluon/cluon.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <string>

TEST_CASE("Testing MyTestMessage0.") {
    testdata::MyTestMessage0 tmp;
    REQUIRE(tmp.attribute1());
    REQUIRE('c' == tmp.attribute2());

    tmp.attribute2('d');

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();
    REQUIRE(26 == s.size());

    REQUIRE(0x82 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xc3 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(20)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(21)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(22)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(23)));
    REQUIRE(0xa1 == static_cast<uint8_t>(s.at(24)));
    REQUIRE(0x64 == static_cast<uint8_t>(s.at(25)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage0 tmp2;
    REQUIRE(tmp2.attribute1());
    REQUIRE('c' == tmp2.attribute2());

    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
    REQUIRE(tmp2.attribute2() == tmp.attribute2());
}

TEST_CASE("Testing MyTestMessage0 with false value.") {
    testdata::MyTestMessage0 tmp;
    REQUIRE(tmp.attribute1());
    REQUIRE('c' == tmp.attribute2());

    tmp.attribute1(false).attribute2('d');

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();
    REQUIRE(26 == s.size());

    REQUIRE(0x82 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xc2 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(20)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(21)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(22)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(23)));
    REQUIRE(0xa1 == static_cast<uint8_t>(s.at(24)));
    REQUIRE(0x64 == static_cast<uint8_t>(s.at(25)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage0 tmp2;
    REQUIRE(tmp2.attribute1());
    REQUIRE('c' == tmp2.attribute2());

    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
    REQUIRE(tmp2.attribute2() == tmp.attribute2());
}

TEST_CASE("Testing MyTestMessage2 with v <= 0x7F.") {
    testdata::MyTestMessage2 tmp;
    REQUIRE(123 == tmp.attribute1());

    tmp.attribute1(22);
    REQUIRE(22 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(13 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0x16 == static_cast<uint8_t>(s.at(12)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage2 tmp2;
    REQUIRE(123 == tmp2.attribute1());

    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage2 with v <= 0xFF.") {
    testdata::MyTestMessage2 tmp;
    REQUIRE(123 == tmp.attribute1());

    tmp.attribute1(234);
    REQUIRE(234 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(14 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xCC == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xEA == static_cast<uint8_t>(s.at(13)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage2 tmp2;
    REQUIRE(123 == tmp2.attribute1());

    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage8 with v <= 0xFFFF.") {
    testdata::MyTestMessage8 tmp;
    REQUIRE(123 == tmp.attribute1());

    tmp.attribute1(12345);
    REQUIRE(12345 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(15 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xCD == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0x30 == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x39 == static_cast<uint8_t>(s.at(14)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage8 tmp2;
    REQUIRE(123 == tmp2.attribute1());

    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage8 with v <= 0xFFFFFFFF.") {
    testdata::MyTestMessage8 tmp;
    REQUIRE(123 == tmp.attribute1());

    tmp.attribute1(123456789);
    REQUIRE(123456789 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(17 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xCE == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0x07 == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x5B == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0xCD == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x15 == static_cast<uint8_t>(s.at(16)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage8 tmp2;
    REQUIRE(123 == tmp2.attribute1());

    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage8 with v > 0xFFFFFFFF.") {
    testdata::MyTestMessage8 tmp;
    REQUIRE(123 == tmp.attribute1());

    tmp.attribute1(std::numeric_limits<uint64_t>::max() - 1);
    REQUIRE(std::numeric_limits<uint64_t>::max() - 1 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(21 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xcf == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xff == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0xff == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0xff == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0xff == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0xff == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0xff == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0xff == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0xfe == static_cast<uint8_t>(s.at(20)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage8 tmp2;
    REQUIRE(123 == tmp2.attribute1());

    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage9.") {
    testdata::MyTestMessage9 tmp;

    tmp.attribute1(1.1f);
    tmp.attribute2(1.23456789);
    REQUIRE(1.1f == Approx(tmp.attribute1()));
    REQUIRE(1.23456789 == Approx(tmp.attribute2()));

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(37 == s.size());

    REQUIRE(0x82 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xca == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0x3f == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x8c == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0xcc == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(20)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(21)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(22)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(23)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(24)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(25)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(26)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(27)));
    REQUIRE(0xcb == static_cast<uint8_t>(s.at(28)));
    REQUIRE(0x3f == static_cast<uint8_t>(s.at(29)));
    REQUIRE(0xf3 == static_cast<uint8_t>(s.at(30)));
    REQUIRE(0xc0 == static_cast<uint8_t>(s.at(31)));
    REQUIRE(0xca == static_cast<uint8_t>(s.at(32)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(33)));
    REQUIRE(0x83 == static_cast<uint8_t>(s.at(34)));
    REQUIRE(0xde == static_cast<uint8_t>(s.at(35)));
    REQUIRE(0x1b == static_cast<uint8_t>(s.at(36)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage9 tmp2;
    tmp2.accept(msgPackDecoder);

    REQUIRE(1.1f == Approx(tmp2.attribute1()));
    REQUIRE(1.23456789 == Approx(tmp2.attribute2()));
}

TEST_CASE("Testing MyTestMessage10 with v = -12.") {
    testdata::MyTestMessage10 tmp;

    tmp.attribute1(-12);
    REQUIRE(-12 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(13 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xF4 == static_cast<uint8_t>(s.at(12)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage10 tmp2;
    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage10 with v = -123.") {
    testdata::MyTestMessage10 tmp;

    tmp.attribute1(-123);
    REQUIRE(-123 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(14 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xD0 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0x85 == static_cast<uint8_t>(s.at(13)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage10 tmp2;
    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage10 with v = -290.") {
    testdata::MyTestMessage10 tmp;

    tmp.attribute1(-290);
    REQUIRE(-290 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(15 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xD1 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xFE == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0xDE == static_cast<uint8_t>(s.at(14)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage10 tmp2;
    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage10 with v = -123456.") {
    testdata::MyTestMessage10 tmp;

    tmp.attribute1(-123456);
    REQUIRE(-123456 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(17 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xD2 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xFF == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0xFE == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x1D == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0xC0 == static_cast<uint8_t>(s.at(16)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage10 tmp2;
    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage10 with v > -0xFFFFFFFF.") {
    testdata::MyTestMessage10 tmp;

    tmp.attribute1(std::numeric_limits<int64_t>::lowest() + 1);
    REQUIRE(std::numeric_limits<int64_t>::lowest() + 1 == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(21 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xd3 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0x80 == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x0 == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x0 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x0 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0x0 == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x0 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x0 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x1 == static_cast<uint8_t>(s.at(20)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage10 tmp2;
    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
}

TEST_CASE("Testing MyTestMessage3.") {
    testdata::MyTestMessage3 tmp;
    REQUIRE(124 == tmp.attribute1());
    REQUIRE(-124 == tmp.attribute2());

    tmp.attribute1(123).attribute2(-123);
    REQUIRE(123 == tmp.attribute1());
    REQUIRE(-123 == tmp.attribute2());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(26 == s.size());

    REQUIRE(0x82 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0x7b == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(20)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(21)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(22)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(23)));
    REQUIRE(0xd0 == static_cast<uint8_t>(s.at(24)));
    REQUIRE(0x85 == static_cast<uint8_t>(s.at(25)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage3 tmp2;
    REQUIRE(124 == tmp2.attribute1());
    REQUIRE(-124 == tmp2.attribute2());

    tmp2.accept(msgPackDecoder);

    REQUIRE(tmp2.attribute1() == tmp.attribute1());
    REQUIRE(tmp2.attribute2() == tmp.attribute2());
}

TEST_CASE("Testing MyTestMessage4.") {
    testdata::MyTestMessage4 tmp;
    REQUIRE(tmp.attribute1().empty());

    tmp.attribute1("testing");
    REQUIRE("testing" == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(20 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xa7 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x73 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x6e == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x67 == static_cast<uint8_t>(s.at(19)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage4 tmp2;
    REQUIRE(tmp2.attribute1().empty());

    tmp2.accept(msgPackDecoder);

    REQUIRE("testing" == tmp2.attribute1());
}

TEST_CASE("Testing MyTestMessage4 larger than 0xbf.") {
    testdata::MyTestMessage4 tmp;
    REQUIRE(tmp.attribute1().empty());

    tmp.attribute1("ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHI"
                   "JABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ12");
    REQUIRE("ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEF"
            "GHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ12"
            == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(206 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0xd9 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xc0 == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(20)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(21)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(22)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(23)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(24)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(25)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(26)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(27)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(28)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(29)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(30)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(31)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(32)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(33)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(34)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(35)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(36)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(37)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(38)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(39)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(40)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(41)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(42)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(43)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(44)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(45)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(46)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(47)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(48)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(49)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(50)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(51)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(52)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(53)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(54)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(55)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(56)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(57)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(58)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(59)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(60)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(61)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(62)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(63)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(64)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(65)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(66)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(67)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(68)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(69)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(70)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(71)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(72)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(73)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(74)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(75)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(76)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(77)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(78)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(79)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(80)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(81)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(82)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(83)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(84)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(85)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(86)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(87)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(88)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(89)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(90)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(91)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(92)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(93)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(94)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(95)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(96)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(97)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(98)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(99)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(100)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(101)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(102)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(103)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(104)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(105)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(106)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(107)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(108)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(109)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(110)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(111)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(112)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(113)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(114)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(115)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(116)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(117)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(118)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(119)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(120)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(121)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(122)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(123)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(124)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(125)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(126)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(127)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(128)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(129)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(130)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(131)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(132)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(133)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(134)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(135)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(136)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(137)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(138)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(139)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(140)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(141)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(142)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(143)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(144)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(145)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(146)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(147)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(148)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(149)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(150)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(151)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(152)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(153)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(154)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(155)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(156)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(157)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(158)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(159)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(160)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(161)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(162)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(163)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(164)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(165)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(166)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(167)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(168)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(169)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(170)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(171)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(172)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(173)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(174)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(175)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(176)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(177)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(178)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(179)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(180)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(181)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(182)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(183)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(184)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(185)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(186)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(187)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(188)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(189)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(190)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(191)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(192)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(193)));
    REQUIRE(0x41 == static_cast<uint8_t>(s.at(194)));
    REQUIRE(0x42 == static_cast<uint8_t>(s.at(195)));
    REQUIRE(0x43 == static_cast<uint8_t>(s.at(196)));
    REQUIRE(0x44 == static_cast<uint8_t>(s.at(197)));
    REQUIRE(0x45 == static_cast<uint8_t>(s.at(198)));
    REQUIRE(0x46 == static_cast<uint8_t>(s.at(199)));
    REQUIRE(0x47 == static_cast<uint8_t>(s.at(200)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(201)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(202)));
    REQUIRE(0x4a == static_cast<uint8_t>(s.at(203)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(204)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(205)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage4 tmp2;
    REQUIRE(tmp2.attribute1().empty());

    tmp2.accept(msgPackDecoder);

    REQUIRE("ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEF"
            "GHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ12"
            == tmp2.attribute1());
}

TEST_CASE("Testing MyTestMessage4 of size 0xda.") {
    testdata::MyTestMessage4 tmp;
    REQUIRE(tmp.attribute1().empty());

    std::string value;
    for (uint16_t i = 0; i < std::numeric_limits<uint16_t>::max(); i++) { value += "A"; }

    tmp.attribute1(value);
    REQUIRE(value == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(65550 == s.size());
    REQUIRE(0xda == static_cast<uint8_t>(s.at(12)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage4 tmp2;
    REQUIRE(tmp2.attribute1().empty());

    tmp2.accept(msgPackDecoder);

    REQUIRE(value == tmp2.attribute1());
}

TEST_CASE("Testing MyTestMessage4 of size 0xdb.") {
    testdata::MyTestMessage4 tmp;
    REQUIRE(tmp.attribute1().empty());

    std::string value;
    for (uint32_t i = 0; i < std::numeric_limits<uint16_t>::max() + 2; i++) { value += "A"; }

    tmp.attribute1(value);
    REQUIRE(value == tmp.attribute1());

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(65554 == s.size());
    REQUIRE(0xdb == static_cast<uint8_t>(s.at(12)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage4 tmp2;
    REQUIRE(tmp2.attribute1().empty());

    tmp2.accept(msgPackDecoder);

    REQUIRE(value == tmp2.attribute1());
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

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp.accept(msgPackEncoder);

    std::string s = msgPackEncoder.encodedData();

    REQUIRE(176 == s.size());

    REQUIRE(0x8b == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(20)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(21)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(22)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(23)));
    REQUIRE(0xfd == static_cast<uint8_t>(s.at(24)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(25)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(26)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(27)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(28)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(29)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(30)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(31)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(32)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(33)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(34)));
    REQUIRE(0x33 == static_cast<uint8_t>(s.at(35)));
    REQUIRE(0x67 == static_cast<uint8_t>(s.at(36)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(37)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(38)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(39)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(40)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(41)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(42)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(43)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(44)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(45)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(46)));
    REQUIRE(0x34 == static_cast<uint8_t>(s.at(47)));
    REQUIRE(0xd0 == static_cast<uint8_t>(s.at(48)));
    REQUIRE(0x99 == static_cast<uint8_t>(s.at(49)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(50)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(51)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(52)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(53)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(54)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(55)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(56)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(57)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(58)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(59)));
    REQUIRE(0x35 == static_cast<uint8_t>(s.at(60)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(61)));
    REQUIRE(0x27 == static_cast<uint8_t>(s.at(62)));
    REQUIRE(0x13 == static_cast<uint8_t>(s.at(63)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(64)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(65)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(66)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(67)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(68)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(69)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(70)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(71)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(72)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(73)));
    REQUIRE(0x36 == static_cast<uint8_t>(s.at(74)));
    REQUIRE(0xd1 == static_cast<uint8_t>(s.at(75)));
    REQUIRE(0xd8 == static_cast<uint8_t>(s.at(76)));
    REQUIRE(0xed == static_cast<uint8_t>(s.at(77)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(78)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(79)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(80)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(81)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(82)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(83)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(84)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(85)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(86)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(87)));
    REQUIRE(0x37 == static_cast<uint8_t>(s.at(88)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(89)));
    REQUIRE(0xd4 == static_cast<uint8_t>(s.at(90)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(91)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(92)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(93)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(94)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(95)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(96)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(97)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(98)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(99)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(100)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(101)));
    REQUIRE(0x38 == static_cast<uint8_t>(s.at(102)));
    REQUIRE(0xd2 == static_cast<uint8_t>(s.at(103)));
    REQUIRE(0xff == static_cast<uint8_t>(s.at(104)));
    REQUIRE(0xfe == static_cast<uint8_t>(s.at(105)));
    REQUIRE(0xdd == static_cast<uint8_t>(s.at(106)));
    REQUIRE(0xaf == static_cast<uint8_t>(s.at(107)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(108)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(109)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(110)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(111)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(112)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(113)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(114)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(115)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(116)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(117)));
    REQUIRE(0x39 == static_cast<uint8_t>(s.at(118)));
    REQUIRE(0xca == static_cast<uint8_t>(s.at(119)));
    REQUIRE(0xc0 == static_cast<uint8_t>(s.at(120)));
    REQUIRE(0xad == static_cast<uint8_t>(s.at(121)));
    REQUIRE(0xd3 == static_cast<uint8_t>(s.at(122)));
    REQUIRE(0xc3 == static_cast<uint8_t>(s.at(123)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(124)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(125)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(126)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(127)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(128)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(129)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(130)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(131)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(132)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(133)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(134)));
    REQUIRE(0x30 == static_cast<uint8_t>(s.at(135)));
    REQUIRE(0xcb == static_cast<uint8_t>(s.at(136)));
    REQUIRE(0xc0 == static_cast<uint8_t>(s.at(137)));
    REQUIRE(0x49 == static_cast<uint8_t>(s.at(138)));
    REQUIRE(0x37 == static_cast<uint8_t>(s.at(139)));
    REQUIRE(0x4f == static_cast<uint8_t>(s.at(140)));
    REQUIRE(0xd == static_cast<uint8_t>(s.at(141)));
    REQUIRE(0x84 == static_cast<uint8_t>(s.at(142)));
    REQUIRE(0x4d == static_cast<uint8_t>(s.at(143)));
    REQUIRE(0x1 == static_cast<uint8_t>(s.at(144)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(145)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(146)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(147)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(148)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(149)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(150)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(151)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(152)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(153)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(154)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(155)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(156)));
    REQUIRE(0xb2 == static_cast<uint8_t>(s.at(157)));
    REQUIRE(0x48 == static_cast<uint8_t>(s.at(158)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(159)));
    REQUIRE(0x6c == static_cast<uint8_t>(s.at(160)));
    REQUIRE(0x6c == static_cast<uint8_t>(s.at(161)));
    REQUIRE(0x6f == static_cast<uint8_t>(s.at(162)));
    REQUIRE(0x20 == static_cast<uint8_t>(s.at(163)));
    REQUIRE(0x63 == static_cast<uint8_t>(s.at(164)));
    REQUIRE(0x6c == static_cast<uint8_t>(s.at(165)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(166)));
    REQUIRE(0x6f == static_cast<uint8_t>(s.at(167)));
    REQUIRE(0x6e == static_cast<uint8_t>(s.at(168)));
    REQUIRE(0x20 == static_cast<uint8_t>(s.at(169)));
    REQUIRE(0x57 == static_cast<uint8_t>(s.at(170)));
    REQUIRE(0x6f == static_cast<uint8_t>(s.at(171)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(172)));
    REQUIRE(0x6c == static_cast<uint8_t>(s.at(173)));
    REQUIRE(0x64 == static_cast<uint8_t>(s.at(174)));
    REQUIRE(0x21 == static_cast<uint8_t>(s.at(175)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

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

    tmp2.accept(msgPackDecoder);

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

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp6.accept(msgPackEncoder);
    std::string s = msgPackEncoder.encodedData();

    REQUIRE(26 == s.size());

    REQUIRE(0x81 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0x81 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(20)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(21)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(22)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(23)));
    REQUIRE(0xcc == static_cast<uint8_t>(s.at(24)));
    REQUIRE(0x96 == static_cast<uint8_t>(s.at(25)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage6 tmp6_2;
    REQUIRE(123 == tmp6_2.attribute1().attribute1());
    tmp6_2.accept(msgPackDecoder);
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

    cluon::ToMsgPackVisitor msgPackEncoder;
    tmp7.accept(msgPackEncoder);
    std::string s = msgPackEncoder.encodedData();

    REQUIRE(61 == s.size());

    REQUIRE(0x83 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0x81 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(20)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(21)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(22)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(23)));
    REQUIRE(0x9 == static_cast<uint8_t>(s.at(24)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(25)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(26)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(27)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(28)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(29)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(30)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(31)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(32)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(33)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(34)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(35)));
    REQUIRE(0xc == static_cast<uint8_t>(s.at(36)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(37)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(38)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(39)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(40)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(41)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(42)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(43)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(44)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(45)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(46)));
    REQUIRE(0x33 == static_cast<uint8_t>(s.at(47)));
    REQUIRE(0x81 == static_cast<uint8_t>(s.at(48)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(49)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(50)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(51)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(52)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(53)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(54)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(55)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(56)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(57)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(58)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(59)));
    REQUIRE(0xd == static_cast<uint8_t>(s.at(60)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage7 tmp7_2;
    REQUIRE(123 == tmp7_2.attribute1().attribute1());
    REQUIRE(12345 == tmp7_2.attribute2());
    REQUIRE(123 == tmp7_2.attribute3().attribute1());

    tmp7_2.accept(msgPackDecoder);
    REQUIRE(9 == tmp7_2.attribute1().attribute1());
    REQUIRE(12 == tmp7_2.attribute2());
    REQUIRE(13 == tmp7_2.attribute3().attribute1());

    // Simple toString().
    std::stringstream buffer;
    tmp7_2.accept([](uint32_t, const std::string &, const std::string &) {},
                  [&buffer](uint32_t, std::string &&, std::string &&n, auto v) { buffer << n << " = " << +v << '\n'; },
                  []() {});
    std::cout << buffer.str() << std::endl;
}

class SetValues {
   public:
    uint32_t value{1000};

    void preVisit(uint32_t, const std::string &, const std::string &) noexcept {}
    void postVisit() noexcept {}

    void visit(uint32_t, std::string &&, std::string &&, bool &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, char &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, int8_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, uint8_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, int16_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, uint16_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, int32_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, uint32_t &v) noexcept { v = value++; }
    void visit(uint32_t, std::string &&, std::string &&, int64_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, uint64_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, float &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, double &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, std::string &) noexcept {}

    template <typename T>
    void visit(uint32_t &id, std::string &&typeName, std::string &&name, T &) noexcept {}
};

class ValueSummarizer {
   public:
    uint32_t value{0};

    void preVisit(uint32_t, const std::string &, const std::string &) noexcept {}
    void postVisit() noexcept {}

    void visit(uint32_t, std::string &&, std::string &&, bool &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, char &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, int8_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, uint8_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, int16_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, uint16_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, int32_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, uint32_t &v) noexcept { value += v; }
    void visit(uint32_t, std::string &&, std::string &&, int64_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, uint64_t &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, float &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, double &) noexcept {}
    void visit(uint32_t, std::string &&, std::string &&, std::string &) noexcept {}

    template <typename T>
    void visit(uint32_t &id, std::string &&typeName, std::string &&name, T &) noexcept {}
};

TEST_CASE("Testing more than 0xF and less than 0xFFFF fields.") {
    std::stringstream msg;

    msg << "message MyTestMessage [id = 1] {" << std::endl;
    for (uint32_t i{0}; i < 32; i++) {
        msg << "    uint32 attribute" << (i+1) << " [ default = " << i << ", id = " << (i+1) << " ];" << std::endl;
    }
    msg << "}" << std::endl;

    cluon::MessageParser mp;
    auto retVal = mp.parse(msg.str());
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    auto listOfMessages = retVal.first;
    REQUIRE(1 == listOfMessages.size());

    cluon::MetaMessage m = listOfMessages[0];
    REQUIRE(32 == m.listOfMetaFields().size());

    cluon::GenericMessage gm;
    gm.createFrom(m, listOfMessages);

    SetValues setter;
    gm.accept(setter);

    cluon::ToMsgPackVisitor msgPackEncoder;
    gm.accept(msgPackEncoder);
    std::string s = msgPackEncoder.encodedData();
    REQUIRE(474 == s.size());

    REQUIRE(0xde == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0x0 == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x20 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(3)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(4)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(5)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(6)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(7)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(8)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(9)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(10)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(11)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(12)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(13)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(14)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(15)));
    REQUIRE(0xe8 == static_cast<uint8_t>(s.at(16)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(17)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(18)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(19)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(20)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(21)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(22)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(23)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(24)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(25)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(26)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(27)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(28)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(29)));
    REQUIRE(0xe9 == static_cast<uint8_t>(s.at(30)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(31)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(32)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(33)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(34)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(35)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(36)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(37)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(38)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(39)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(40)));
    REQUIRE(0x33 == static_cast<uint8_t>(s.at(41)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(42)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(43)));
    REQUIRE(0xea == static_cast<uint8_t>(s.at(44)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(45)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(46)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(47)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(48)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(49)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(50)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(51)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(52)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(53)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(54)));
    REQUIRE(0x34 == static_cast<uint8_t>(s.at(55)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(56)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(57)));
    REQUIRE(0xeb == static_cast<uint8_t>(s.at(58)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(59)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(60)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(61)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(62)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(63)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(64)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(65)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(66)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(67)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(68)));
    REQUIRE(0x35 == static_cast<uint8_t>(s.at(69)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(70)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(71)));
    REQUIRE(0xec == static_cast<uint8_t>(s.at(72)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(73)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(74)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(75)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(76)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(77)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(78)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(79)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(80)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(81)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(82)));
    REQUIRE(0x36 == static_cast<uint8_t>(s.at(83)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(84)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(85)));
    REQUIRE(0xed == static_cast<uint8_t>(s.at(86)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(87)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(88)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(89)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(90)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(91)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(92)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(93)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(94)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(95)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(96)));
    REQUIRE(0x37 == static_cast<uint8_t>(s.at(97)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(98)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(99)));
    REQUIRE(0xee == static_cast<uint8_t>(s.at(100)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(101)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(102)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(103)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(104)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(105)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(106)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(107)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(108)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(109)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(110)));
    REQUIRE(0x38 == static_cast<uint8_t>(s.at(111)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(112)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(113)));
    REQUIRE(0xef == static_cast<uint8_t>(s.at(114)));
    REQUIRE(0xaa == static_cast<uint8_t>(s.at(115)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(116)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(117)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(118)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(119)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(120)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(121)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(122)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(123)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(124)));
    REQUIRE(0x39 == static_cast<uint8_t>(s.at(125)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(126)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(127)));
    REQUIRE(0xf0 == static_cast<uint8_t>(s.at(128)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(129)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(130)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(131)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(132)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(133)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(134)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(135)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(136)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(137)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(138)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(139)));
    REQUIRE(0x30 == static_cast<uint8_t>(s.at(140)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(141)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(142)));
    REQUIRE(0xf1 == static_cast<uint8_t>(s.at(143)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(144)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(145)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(146)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(147)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(148)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(149)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(150)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(151)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(152)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(153)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(154)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(155)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(156)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(157)));
    REQUIRE(0xf2 == static_cast<uint8_t>(s.at(158)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(159)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(160)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(161)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(162)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(163)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(164)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(165)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(166)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(167)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(168)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(169)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(170)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(171)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(172)));
    REQUIRE(0xf3 == static_cast<uint8_t>(s.at(173)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(174)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(175)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(176)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(177)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(178)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(179)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(180)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(181)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(182)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(183)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(184)));
    REQUIRE(0x33 == static_cast<uint8_t>(s.at(185)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(186)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(187)));
    REQUIRE(0xf4 == static_cast<uint8_t>(s.at(188)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(189)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(190)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(191)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(192)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(193)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(194)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(195)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(196)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(197)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(198)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(199)));
    REQUIRE(0x34 == static_cast<uint8_t>(s.at(200)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(201)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(202)));
    REQUIRE(0xf5 == static_cast<uint8_t>(s.at(203)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(204)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(205)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(206)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(207)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(208)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(209)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(210)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(211)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(212)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(213)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(214)));
    REQUIRE(0x35 == static_cast<uint8_t>(s.at(215)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(216)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(217)));
    REQUIRE(0xf6 == static_cast<uint8_t>(s.at(218)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(219)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(220)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(221)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(222)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(223)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(224)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(225)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(226)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(227)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(228)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(229)));
    REQUIRE(0x36 == static_cast<uint8_t>(s.at(230)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(231)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(232)));
    REQUIRE(0xf7 == static_cast<uint8_t>(s.at(233)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(234)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(235)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(236)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(237)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(238)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(239)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(240)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(241)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(242)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(243)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(244)));
    REQUIRE(0x37 == static_cast<uint8_t>(s.at(245)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(246)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(247)));
    REQUIRE(0xf8 == static_cast<uint8_t>(s.at(248)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(249)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(250)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(251)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(252)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(253)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(254)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(255)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(256)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(257)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(258)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(259)));
    REQUIRE(0x38 == static_cast<uint8_t>(s.at(260)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(261)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(262)));
    REQUIRE(0xf9 == static_cast<uint8_t>(s.at(263)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(264)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(265)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(266)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(267)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(268)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(269)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(270)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(271)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(272)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(273)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(274)));
    REQUIRE(0x39 == static_cast<uint8_t>(s.at(275)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(276)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(277)));
    REQUIRE(0xfa == static_cast<uint8_t>(s.at(278)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(279)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(280)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(281)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(282)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(283)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(284)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(285)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(286)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(287)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(288)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(289)));
    REQUIRE(0x30 == static_cast<uint8_t>(s.at(290)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(291)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(292)));
    REQUIRE(0xfb == static_cast<uint8_t>(s.at(293)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(294)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(295)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(296)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(297)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(298)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(299)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(300)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(301)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(302)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(303)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(304)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(305)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(306)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(307)));
    REQUIRE(0xfc == static_cast<uint8_t>(s.at(308)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(309)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(310)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(311)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(312)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(313)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(314)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(315)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(316)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(317)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(318)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(319)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(320)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(321)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(322)));
    REQUIRE(0xfd == static_cast<uint8_t>(s.at(323)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(324)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(325)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(326)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(327)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(328)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(329)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(330)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(331)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(332)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(333)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(334)));
    REQUIRE(0x33 == static_cast<uint8_t>(s.at(335)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(336)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(337)));
    REQUIRE(0xfe == static_cast<uint8_t>(s.at(338)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(339)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(340)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(341)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(342)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(343)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(344)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(345)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(346)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(347)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(348)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(349)));
    REQUIRE(0x34 == static_cast<uint8_t>(s.at(350)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(351)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(352)));
    REQUIRE(0xff == static_cast<uint8_t>(s.at(353)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(354)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(355)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(356)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(357)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(358)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(359)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(360)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(361)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(362)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(363)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(364)));
    REQUIRE(0x35 == static_cast<uint8_t>(s.at(365)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(366)));
    REQUIRE(0x4 == static_cast<uint8_t>(s.at(367)));
    REQUIRE(0x0 == static_cast<uint8_t>(s.at(368)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(369)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(370)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(371)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(372)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(373)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(374)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(375)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(376)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(377)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(378)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(379)));
    REQUIRE(0x36 == static_cast<uint8_t>(s.at(380)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(381)));
    REQUIRE(0x4 == static_cast<uint8_t>(s.at(382)));
    REQUIRE(0x1 == static_cast<uint8_t>(s.at(383)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(384)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(385)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(386)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(387)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(388)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(389)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(390)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(391)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(392)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(393)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(394)));
    REQUIRE(0x37 == static_cast<uint8_t>(s.at(395)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(396)));
    REQUIRE(0x4 == static_cast<uint8_t>(s.at(397)));
    REQUIRE(0x2 == static_cast<uint8_t>(s.at(398)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(399)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(400)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(401)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(402)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(403)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(404)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(405)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(406)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(407)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(408)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(409)));
    REQUIRE(0x38 == static_cast<uint8_t>(s.at(410)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(411)));
    REQUIRE(0x4 == static_cast<uint8_t>(s.at(412)));
    REQUIRE(0x3 == static_cast<uint8_t>(s.at(413)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(414)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(415)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(416)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(417)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(418)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(419)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(420)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(421)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(422)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(423)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(424)));
    REQUIRE(0x39 == static_cast<uint8_t>(s.at(425)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(426)));
    REQUIRE(0x4 == static_cast<uint8_t>(s.at(427)));
    REQUIRE(0x4 == static_cast<uint8_t>(s.at(428)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(429)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(430)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(431)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(432)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(433)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(434)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(435)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(436)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(437)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(438)));
    REQUIRE(0x33 == static_cast<uint8_t>(s.at(439)));
    REQUIRE(0x30 == static_cast<uint8_t>(s.at(440)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(441)));
    REQUIRE(0x4 == static_cast<uint8_t>(s.at(442)));
    REQUIRE(0x5 == static_cast<uint8_t>(s.at(443)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(444)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(445)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(446)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(447)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(448)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(449)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(450)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(451)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(452)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(453)));
    REQUIRE(0x33 == static_cast<uint8_t>(s.at(454)));
    REQUIRE(0x31 == static_cast<uint8_t>(s.at(455)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(456)));
    REQUIRE(0x4 == static_cast<uint8_t>(s.at(457)));
    REQUIRE(0x6 == static_cast<uint8_t>(s.at(458)));
    REQUIRE(0xab == static_cast<uint8_t>(s.at(459)));
    REQUIRE(0x61 == static_cast<uint8_t>(s.at(460)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(461)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(462)));
    REQUIRE(0x72 == static_cast<uint8_t>(s.at(463)));
    REQUIRE(0x69 == static_cast<uint8_t>(s.at(464)));
    REQUIRE(0x62 == static_cast<uint8_t>(s.at(465)));
    REQUIRE(0x75 == static_cast<uint8_t>(s.at(466)));
    REQUIRE(0x74 == static_cast<uint8_t>(s.at(467)));
    REQUIRE(0x65 == static_cast<uint8_t>(s.at(468)));
    REQUIRE(0x33 == static_cast<uint8_t>(s.at(469)));
    REQUIRE(0x32 == static_cast<uint8_t>(s.at(470)));
    REQUIRE(0xcd == static_cast<uint8_t>(s.at(471)));
    REQUIRE(0x4 == static_cast<uint8_t>(s.at(472)));
    REQUIRE(0x7 == static_cast<uint8_t>(s.at(473)));

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    cluon::GenericMessage gm2;
    gm2.createFrom(m, listOfMessages);
    gm2.accept(msgPackDecoder);

    ValueSummarizer vs;
    gm2.accept(vs);

    REQUIRE(32496 == vs.value);

//int i = 0;
//for(auto c : s) {
////    std::cout << "0x" << std::hex << (uint32_t)(uint8_t)c << " ";
//    std::cout << "REQUIRE(0x" << std::hex << (uint32_t)(uint8_t)c << " == static_cast<uint8_t>(s.at(" << std::dec << i++ << ")));" << std::endl;
//}
//std::cout << std::endl;
}

