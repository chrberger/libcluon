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
#include "cluon/ToMsgPackVisitor.hpp"
#include "cluon/cluon.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <iomanip>
#include <iostream>
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

//int i = 0;
//for(auto c : s) {
////    std::cout << "0x" << std::hex << (uint32_t)(uint8_t)c << " ";
//    std::cout << "REQUIRE(0x" << std::hex << (uint32_t)(uint8_t)c << " == static_cast<uint8_t>(s.at(" << std::dec << i++ << ")));" << std::endl;
//}
//std::cout << std::endl;

    std::stringstream sstr{s};
    cluon::FromMsgPackVisitor msgPackDecoder;
    msgPackDecoder.decodeFrom(sstr);

    testdata::MyTestMessage0 tmp2;
    REQUIRE(tmp2.attribute1());
    REQUIRE('c' == tmp2.attribute2());

//    tmp2.accept(msgPackDecoder);

//    REQUIRE(tmp2.attribute1() == tmp.attribute1());
//    REQUIRE(tmp2.attribute2() == tmp.attribute2());
}

//TEST_CASE("Testing MyTestMessage2.") {
//    testdata::MyTestMessage2 tmp;
//    REQUIRE(123 == tmp.attribute1());

//    tmp.attribute1(150);
//    REQUIRE(150 == tmp.attribute1());

//    cluon::ToMsgPackVisitor protoEncoder;
//    tmp.accept(protoEncoder);

//    std::string s = protoEncoder.encodedData();
//    REQUIRE(3 == s.size());

//    REQUIRE(0x8 == static_cast<uint8_t>(s.at(0)));
//    REQUIRE(0x96 == static_cast<uint8_t>(s.at(1)));
//    REQUIRE(0x1 == static_cast<uint8_t>(s.at(2)));

//    std::stringstream sstr{s};
//    cluon::FromMsgPackVisitor protoDecoder;
//    protoDecoder.decodeFrom(sstr);

//    testdata::MyTestMessage2 tmp2;
//    REQUIRE(123 == tmp2.attribute1());

//    tmp2.accept(protoDecoder);

//    REQUIRE(tmp2.attribute1() == tmp.attribute1());
//}

//TEST_CASE("Testing MyTestMessage3.") {
//    testdata::MyTestMessage3 tmp;
//    REQUIRE(124 == tmp.attribute1());
//    REQUIRE(-124 == tmp.attribute2());

//    tmp.attribute1(123).attribute2(-123);
//    REQUIRE(123 == tmp.attribute1());
//    REQUIRE(-123 == tmp.attribute2());

//    cluon::ToMsgPackVisitor protoEncoder;
//    tmp.accept(protoEncoder);

//    std::string s = protoEncoder.encodedData();
//    REQUIRE(5 == s.size());

//    REQUIRE(0x8 == static_cast<uint8_t>(s.at(0)));
//    REQUIRE(0x7b == static_cast<uint8_t>(s.at(1)));
//    REQUIRE(0x10 == static_cast<uint8_t>(s.at(2)));
//    REQUIRE(0xf5 == static_cast<uint8_t>(s.at(3)));
//    REQUIRE(0x1 == static_cast<uint8_t>(s.at(4)));

//    std::stringstream sstr{s};
//    cluon::FromMsgPackVisitor protoDecoder;
//    protoDecoder.decodeFrom(sstr);

//    testdata::MyTestMessage3 tmp2;
//    REQUIRE(124 == tmp2.attribute1());
//    REQUIRE(-124 == tmp2.attribute2());

//    tmp2.accept(protoDecoder);

//    REQUIRE(tmp2.attribute1() == tmp.attribute1());
//    REQUIRE(tmp2.attribute2() == tmp.attribute2());
//}

//TEST_CASE("Testing MyTestMessage4.") {
//    testdata::MyTestMessage4 tmp;
//    REQUIRE(tmp.attribute1().empty());

//    tmp.attribute1("testing");
//    REQUIRE("testing" == tmp.attribute1());

//    cluon::ToMsgPackVisitor protoEncoder;
//    tmp.accept(protoEncoder);

//    std::string s = protoEncoder.encodedData();
//    REQUIRE(9 == s.size());

//    REQUIRE(0x12 == static_cast<uint8_t>(s.at(0)));
//    REQUIRE(0x7 == static_cast<uint8_t>(s.at(1)));
//    REQUIRE(0x74 == static_cast<uint8_t>(s.at(2)));
//    REQUIRE(0x65 == static_cast<uint8_t>(s.at(3)));
//    REQUIRE(0x73 == static_cast<uint8_t>(s.at(4)));
//    REQUIRE(0x74 == static_cast<uint8_t>(s.at(5)));
//    REQUIRE(0x69 == static_cast<uint8_t>(s.at(6)));
//    REQUIRE(0x6e == static_cast<uint8_t>(s.at(7)));
//    REQUIRE(0x67 == static_cast<uint8_t>(s.at(8)));

//    std::stringstream sstr{s};
//    cluon::FromMsgPackVisitor protoDecoder;
//    protoDecoder.decodeFrom(sstr);

//    testdata::MyTestMessage4 tmp2;
//    REQUIRE(tmp2.attribute1().empty());

//    tmp2.accept(protoDecoder);

//    REQUIRE("testing" == tmp2.attribute1());
//}

//TEST_CASE("Testing MyTestMessage5.") { // NOLINT
//    testdata::MyTestMessage5 tmp;

//    REQUIRE(1 == tmp.attribute1());
//    REQUIRE(-1 == tmp.attribute2());
//    REQUIRE(100 == tmp.attribute3());
//    REQUIRE(-100 == tmp.attribute4());
//    REQUIRE(10000 == tmp.attribute5());
//    REQUIRE(-10000 == tmp.attribute6());
//    REQUIRE(12345 == tmp.attribute7());
//    REQUIRE(-12345 == tmp.attribute8());
//    REQUIRE(-1.2345f == Approx(tmp.attribute9()));
//    REQUIRE(-10.2345 == Approx(tmp.attribute10()));
//    REQUIRE("Hello World!" == tmp.attribute11());

//    tmp.attribute1(3)
//        .attribute2(-3)
//        .attribute3(103)
//        .attribute4(-103)
//        .attribute5(10003)
//        .attribute6(-10003)
//        .attribute7(54321)
//        .attribute8(-54321)
//        .attribute9(-5.4321f)
//        .attribute10(-50.4321)
//        .attribute11("Hello cluon World!");

//    REQUIRE(3 == tmp.attribute1());
//    REQUIRE(-3 == tmp.attribute2());
//    REQUIRE(103 == tmp.attribute3());
//    REQUIRE(-103 == tmp.attribute4());
//    REQUIRE(10003 == tmp.attribute5());
//    REQUIRE(-10003 == tmp.attribute6());
//    REQUIRE(54321 == tmp.attribute7());
//    REQUIRE(-54321 == tmp.attribute8());
//    REQUIRE(-5.4321f == Approx(tmp.attribute9()));
//    REQUIRE(-50.4321 == Approx(tmp.attribute10()));
//    REQUIRE("Hello cluon World!" == tmp.attribute11());

//    cluon::ToMsgPackVisitor protoEncoder;
//    tmp.accept(protoEncoder);

//    std::string s = protoEncoder.encodedData();
//    REQUIRE(58 == s.size());

//    REQUIRE(0x8 == static_cast<uint8_t>(s.at(0)));
//    REQUIRE(0x3 == static_cast<uint8_t>(s.at(1)));
//    REQUIRE(0x10 == static_cast<uint8_t>(s.at(2)));
//    REQUIRE(0x5 == static_cast<uint8_t>(s.at(3)));
//    REQUIRE(0x18 == static_cast<uint8_t>(s.at(4)));
//    REQUIRE(0x67 == static_cast<uint8_t>(s.at(5)));
//    REQUIRE(0x20 == static_cast<uint8_t>(s.at(6)));
//    REQUIRE(0xcd == static_cast<uint8_t>(s.at(7)));
//    REQUIRE(0x1 == static_cast<uint8_t>(s.at(8)));
//    REQUIRE(0x28 == static_cast<uint8_t>(s.at(9)));
//    REQUIRE(0x93 == static_cast<uint8_t>(s.at(10)));
//    REQUIRE(0x4e == static_cast<uint8_t>(s.at(11)));
//    REQUIRE(0x30 == static_cast<uint8_t>(s.at(12)));
//    REQUIRE(0xa5 == static_cast<uint8_t>(s.at(13)));
//    REQUIRE(0x9c == static_cast<uint8_t>(s.at(14)));
//    REQUIRE(0x1 == static_cast<uint8_t>(s.at(15)));
//    REQUIRE(0x38 == static_cast<uint8_t>(s.at(16)));
//    REQUIRE(0xb1 == static_cast<uint8_t>(s.at(17)));
//    REQUIRE(0xa8 == static_cast<uint8_t>(s.at(18)));
//    REQUIRE(0x3 == static_cast<uint8_t>(s.at(19)));
//    REQUIRE(0x40 == static_cast<uint8_t>(s.at(20)));
//    REQUIRE(0xe1 == static_cast<uint8_t>(s.at(21)));
//    REQUIRE(0xd0 == static_cast<uint8_t>(s.at(22)));
//    REQUIRE(0x6 == static_cast<uint8_t>(s.at(23)));
//    REQUIRE(0x4d == static_cast<uint8_t>(s.at(24)));
//    REQUIRE(0xc3 == static_cast<uint8_t>(s.at(25)));
//    REQUIRE(0xd3 == static_cast<uint8_t>(s.at(26)));
//    REQUIRE(0xad == static_cast<uint8_t>(s.at(27)));
//    REQUIRE(0xc0 == static_cast<uint8_t>(s.at(28)));
//    REQUIRE(0x51 == static_cast<uint8_t>(s.at(29)));
//    REQUIRE(0x1 == static_cast<uint8_t>(s.at(30)));
//    REQUIRE(0x4d == static_cast<uint8_t>(s.at(31)));
//    REQUIRE(0x84 == static_cast<uint8_t>(s.at(32)));
//    REQUIRE(0xd == static_cast<uint8_t>(s.at(33)));
//    REQUIRE(0x4f == static_cast<uint8_t>(s.at(34)));
//    REQUIRE(0x37 == static_cast<uint8_t>(s.at(35)));
//    REQUIRE(0x49 == static_cast<uint8_t>(s.at(36)));
//    REQUIRE(0xc0 == static_cast<uint8_t>(s.at(37)));
//    REQUIRE(0x5a == static_cast<uint8_t>(s.at(38)));
//    REQUIRE(0x12 == static_cast<uint8_t>(s.at(39)));
//    REQUIRE(0x48 == static_cast<uint8_t>(s.at(40)));
//    REQUIRE(0x65 == static_cast<uint8_t>(s.at(41)));
//    REQUIRE(0x6c == static_cast<uint8_t>(s.at(42)));
//    REQUIRE(0x6c == static_cast<uint8_t>(s.at(43)));
//    REQUIRE(0x6f == static_cast<uint8_t>(s.at(44)));
//    REQUIRE(0x20 == static_cast<uint8_t>(s.at(45)));
//    REQUIRE(0x63 == static_cast<uint8_t>(s.at(46)));
//    REQUIRE(0x6c == static_cast<uint8_t>(s.at(47)));
//    REQUIRE(0x75 == static_cast<uint8_t>(s.at(48)));
//    REQUIRE(0x6f == static_cast<uint8_t>(s.at(49)));
//    REQUIRE(0x6e == static_cast<uint8_t>(s.at(50)));
//    REQUIRE(0x20 == static_cast<uint8_t>(s.at(51)));
//    REQUIRE(0x57 == static_cast<uint8_t>(s.at(52)));
//    REQUIRE(0x6f == static_cast<uint8_t>(s.at(53)));
//    REQUIRE(0x72 == static_cast<uint8_t>(s.at(54)));
//    REQUIRE(0x6c == static_cast<uint8_t>(s.at(55)));
//    REQUIRE(0x64 == static_cast<uint8_t>(s.at(56)));
//    REQUIRE(0x21 == static_cast<uint8_t>(s.at(57)));

//    std::stringstream sstr{s};
//    cluon::FromMsgPackVisitor protoDecoder;
//    protoDecoder.decodeFrom(sstr);

//    testdata::MyTestMessage5 tmp2;
//    REQUIRE(1 == tmp2.attribute1());
//    REQUIRE(-1 == tmp2.attribute2());
//    REQUIRE(100 == tmp2.attribute3());
//    REQUIRE(-100 == tmp2.attribute4());
//    REQUIRE(10000 == tmp2.attribute5());
//    REQUIRE(-10000 == tmp2.attribute6());
//    REQUIRE(12345 == tmp2.attribute7());
//    REQUIRE(-12345 == tmp2.attribute8());
//    REQUIRE(-1.2345f == Approx(tmp2.attribute9()));
//    REQUIRE(-10.2345 == Approx(tmp2.attribute10()));
//    REQUIRE("Hello World!" == tmp2.attribute11());

//    tmp2.accept(protoDecoder);

//    REQUIRE(tmp2.attribute1() == tmp.attribute1());
//    REQUIRE(tmp2.attribute2() == tmp.attribute2());
//    REQUIRE(tmp2.attribute3() == tmp.attribute3());
//    REQUIRE(tmp2.attribute4() == tmp.attribute4());
//    REQUIRE(tmp2.attribute5() == tmp.attribute5());
//    REQUIRE(tmp2.attribute6() == tmp.attribute6());
//    REQUIRE(tmp2.attribute7() == tmp.attribute7());
//    REQUIRE(tmp2.attribute8() == tmp.attribute8());
//    REQUIRE(tmp2.attribute9() == Approx(tmp.attribute9()));
//    REQUIRE(tmp2.attribute10() == Approx(tmp.attribute10()));
//    REQUIRE(tmp2.attribute11() == tmp.attribute11());
//}

//TEST_CASE("Testing MyTestMessage6 with visitor to visit nested message for serialization and deserialization.") {
//    testdata::MyTestMessage6 tmp6;

//    REQUIRE(123 == tmp6.attribute1().attribute1());

//    testdata::MyTestMessage2 tmp2;
//    tmp6.attribute1(tmp2.attribute1(150));

//    REQUIRE(150 == tmp6.attribute1().attribute1());

//    cluon::ToMsgPackVisitor protoEncoder;
//    tmp6.accept(protoEncoder);
//    std::string s = protoEncoder.encodedData();

//    REQUIRE(5 == s.size());
//    REQUIRE(0x1a == static_cast<uint8_t>(s.at(0)));
//    REQUIRE(0x3 == static_cast<uint8_t>(s.at(1)));
//    REQUIRE(0x8 == static_cast<uint8_t>(s.at(2)));
//    REQUIRE(0x96 == static_cast<uint8_t>(s.at(3)));
//    REQUIRE(0x1 == static_cast<uint8_t>(s.at(4)));

//    std::stringstream sstr{s};
//    cluon::FromMsgPackVisitor protoDecoder;
//    protoDecoder.decodeFrom(sstr);

//    testdata::MyTestMessage6 tmp6_2;
//    REQUIRE(123 == tmp6_2.attribute1().attribute1());
//    tmp6_2.accept(protoDecoder);
//    REQUIRE(150 == tmp6_2.attribute1().attribute1());
//}

//TEST_CASE("Testing MyTestMessage7 with visitor to visit nested messages for serialization and deserialization.") {
//    testdata::MyTestMessage7 tmp7;

//    REQUIRE(123 == tmp7.attribute1().attribute1());
//    REQUIRE(12345 == tmp7.attribute2());
//    REQUIRE(123 == tmp7.attribute3().attribute1());

//    testdata::MyTestMessage2 tmp2_1;
//    tmp7.attribute1(tmp2_1.attribute1(9));

//    tmp7.attribute2(12);

//    testdata::MyTestMessage2 tmp2_3;
//    tmp7.attribute3(tmp2_3.attribute1(13));

//    REQUIRE(9 == tmp7.attribute1().attribute1());
//    REQUIRE(12 == tmp7.attribute2());
//    REQUIRE(13 == tmp7.attribute3().attribute1());

//    cluon::ToMsgPackVisitor protoEncoder;
//    tmp7.accept(protoEncoder);
//    std::string s = protoEncoder.encodedData();

//    REQUIRE(10 == s.size());
//    REQUIRE(0xa == static_cast<uint8_t>(s.at(0)));
//    REQUIRE(0x2 == static_cast<uint8_t>(s.at(1)));
//    REQUIRE(0x8 == static_cast<uint8_t>(s.at(2)));
//    REQUIRE(0x9 == static_cast<uint8_t>(s.at(3)));
//    REQUIRE(0x10 == static_cast<uint8_t>(s.at(4)));
//    REQUIRE(0xc == static_cast<uint8_t>(s.at(5)));
//    REQUIRE(0x1a == static_cast<uint8_t>(s.at(6)));
//    REQUIRE(0x2 == static_cast<uint8_t>(s.at(7)));
//    REQUIRE(0x8 == static_cast<uint8_t>(s.at(8)));
//    REQUIRE(0xd == static_cast<uint8_t>(s.at(9)));

//    std::stringstream sstr{s};
//    cluon::FromMsgPackVisitor protoDecoder;
//    protoDecoder.decodeFrom(sstr);

//    testdata::MyTestMessage7 tmp7_2;
//    REQUIRE(123 == tmp7_2.attribute1().attribute1());
//    REQUIRE(12345 == tmp7_2.attribute2());
//    REQUIRE(123 == tmp7_2.attribute3().attribute1());

//    tmp7_2.accept(protoDecoder);
//    REQUIRE(9 == tmp7_2.attribute1().attribute1());
//    REQUIRE(12 == tmp7_2.attribute2());
//    REQUIRE(13 == tmp7_2.attribute3().attribute1());

//    // Simple toString().
//    std::stringstream buffer;
//    tmp7_2.accept([](uint32_t, const std::string &, const std::string &) {},
//                  [&buffer](uint32_t, std::string &&, std::string &&n, auto v) { buffer << n << " = " << +v << '\n'; },
//                  []() {});
//    std::cout << buffer.str() << std::endl;
//}
