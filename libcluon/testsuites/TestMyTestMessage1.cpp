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
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "cluon/FromProtoVisitor.hpp"
#include "cluon/GenericMessage.hpp"
#include "cluon/MessageParser.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"
#include "cluon/cluonTestDataStructures.hpp"

TEST_CASE("Testing MyTestMessage1.") {
    testdata::MyTestMessage1 tmp;

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
    REQUIRE(-9.5 == Approx(tmp.attribute11()));
    REQUIRE(10.6 == Approx(tmp.attribute12()));
    REQUIRE("Hello World" == tmp.attribute13());
    REQUIRE("Hello Galaxy" == tmp.attribute14());
}

TEST_CASE("Testing MyTestMessage1: copy constructor.") {
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

    testdata::MyTestMessage1 dest(src);
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

TEST_CASE("Testing MyTestMessage1: assignment operator.") {
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

    dest = src;
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

TEST_CASE("Testing MyTestMessage1: copy constructor (move).") {
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

    testdata::MyTestMessage1 dest(std::move(src));
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

TEST_CASE("Testing MyTestMessage1: assignment operator (move).") {
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

    dest = std::move(src);
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

TEST_CASE("Testing MyTestMessage1 with visitor to summarize values.") {
    testdata::MyTestMessage1 tmp;

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
    REQUIRE(-9.5 == Approx(tmp.attribute11()));
    REQUIRE(10.6 == Approx(tmp.attribute12()));
    REQUIRE("Hello World" == tmp.attribute13());
    REQUIRE("Hello Galaxy" == tmp.attribute14());

    //    // This does only work with C++17!
    //    val = 0;
    //    auto summarizer = [&val]<typename T>(T a){
    //        if constexpr (std::is_integral<T>::value) { val += a; };
    //        if constexpr (!std::is_integral<T>::value) { val += 0; };
    //    };
    //    tmp.accept(summarizer);

    class ExtendedSummarizer {
       public:
        void pre(const std::string &_s) { std::cout << "pre = " << _s << std::endl; }

        void post() { std::cout << "post." << std::endl; }

        void sum(int64_t *v, bool /*a*/) { *v += 0; }
        void sum(int64_t *v, char /*a*/) { *v += 0; }
        void sum(int64_t *v, int8_t a) { *v += a; }
        void sum(int64_t *v, uint8_t a) { *v += a; }
        void sum(int64_t *v, int16_t a) { *v += a; }
        void sum(int64_t *v, uint16_t a) { *v += a; }
        void sum(int64_t *v, int32_t a) { *v += a; }
        void sum(int64_t *v, uint32_t a) { *v += a; }
        void sum(int64_t *v, int64_t a) { *v += a; }
        void sum(int64_t *v, uint64_t a) { *v += a; }
        void sum(int64_t *v, float /*a*/) { *v += 0; }
        void sum(int64_t *v, double /*a*/) { *v += 0; }
        void sum(int64_t *v, const std::string & /*a*/) { *v += 0; }
    };

    ExtendedSummarizer es;
    int64_t val = 0;
    tmp.accept([&es](uint32_t, const std::string &, const std::string &_s) { es.pre(_s); },
               [&es, &val](const uint32_t &, std::string &&, std::string &&, auto a) { es.sum(&val, a); },
               [&es]() { es.post(); });

    REQUIRE(4 == val);

    val = 0;
    tmp.accept([](uint32_t, const std::string &, const std::string &) {},
               [&es, &val](uint32_t, std::string &&, std::string &&, auto a) { es.sum(&val, a); },
               []() {});

    REQUIRE(4 == val);

    // Simple toString().
    std::stringstream buffer;
    tmp.accept([](uint32_t, const std::string &, const std::string &) {},
               [&buffer](uint32_t, std::string &&, std::string &&n, auto v) { buffer << n << " = " << v << '\n'; },
               []() {});
    std::cout << buffer.str() << std::endl;
}

TEST_CASE("Dynamically creating messages for simple message.") {
    const char *msg = R"(
// Original message:
// message testdata.MyTestMessage3 [id = 30003] {
//    uint8 attribute1 [ default = 124, id = 1 ];
//    int8 attribute2 [ default = -124, id = 2 ];
// }

message MyMessageA [id = 30003] {
    uint8 field1 [id = 1];
    int8 field2 [id = 2];
}
)";

    testdata::MyTestMessage3 msg3;
    REQUIRE(124 == msg3.attribute1());
    REQUIRE(-124 == msg3.attribute2());

    msg3.attribute1(10).attribute2(20);
    REQUIRE(10 == msg3.attribute1());
    REQUIRE(20 == msg3.attribute2());

    cluon::ToProtoVisitor proto;
    msg3.accept(proto);
    std::string s{proto.encodedData()};

    REQUIRE(4 == s.size());
    REQUIRE(0x8 == static_cast<uint8_t>(s.at(0)));
    REQUIRE(0xa == static_cast<uint8_t>(s.at(1)));
    REQUIRE(0x10 == static_cast<uint8_t>(s.at(2)));
    REQUIRE(0x28 == static_cast<uint8_t>(s.at(3)));

    std::stringstream sstr{s};
    cluon::FromProtoVisitor protoDecoder;
    protoDecoder.decodeFrom(sstr);

    testdata::MyTestMessage3 msg3_2;
    REQUIRE(124 == msg3_2.attribute1());
    REQUIRE(-124 == msg3_2.attribute2());
    msg3_2.accept(protoDecoder);
    REQUIRE(10 == msg3_2.attribute1());
    REQUIRE(20 == msg3_2.attribute2());

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(msg));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    auto listOfMessages = retVal.first;
    REQUIRE(1 == listOfMessages.size());

    cluon::MetaMessage m = listOfMessages.front();

    cluon::GenericMessage gm;
    gm.createFrom(m, listOfMessages);

    REQUIRE(30003 == gm.ID());
    REQUIRE("MyMessageA" == gm.ShortName());
    REQUIRE("MyMessageA" == gm.LongName());

    // Set values in GenericMessage from ProtoDecoder.
    gm.accept(protoDecoder);

    // Turn GenericMessage back into Proto.
    {
        cluon::ToProtoVisitor proto2;
        gm.accept(proto2);
        std::string s2 = proto2.encodedData();
        REQUIRE(4 == s2.size());
        REQUIRE(0x8 == static_cast<uint8_t>(s2.at(0)));
        REQUIRE(0xa == static_cast<uint8_t>(s2.at(1)));
        REQUIRE(0x10 == static_cast<uint8_t>(s2.at(2)));
        REQUIRE(0x28 == static_cast<uint8_t>(s2.at(3)));

        std::stringstream sstr2{s2};
        cluon::FromProtoVisitor protoDecoder2;
        protoDecoder2.decodeFrom(sstr2);

        testdata::MyTestMessage3 msg3_2_2;
        REQUIRE(124 == msg3_2_2.attribute1());
        REQUIRE(-124 == msg3_2_2.attribute2());
        msg3_2_2.accept(protoDecoder2);
        REQUIRE(10 == msg3_2_2.attribute1());
        REQUIRE(20 == msg3_2_2.attribute2());
    }
}

TEST_CASE("Dynamically creating messages for complex message.") {
    const char *msg = R"(
// Original message:
//message testdata.MyTestMessage1 [id = 30001] {
//    bool attribute1 [default = true, id = 1];
//    char attribute2 [default = 'c', id = 2];
//    int8 attribute3 [default = -1, id = 3];
//    uint8 attribute4 [default = 2, id = 4];
//    int16 attribute5 [default = -3, id = 5];
//    uint16 attribute6 [default = 4, id = 6];
//    int32 attribute7 [default = -5, id = 7];
//    uint32 attribute8 [default = 6, id = 8];
//    int64 attribute9 [default = -7, id = 9];
//    uint64 attribute10 [default = 8, id = 10];
//    float attribute11 [default = -9.5, id = 11];
//    double attribute12 [default = 10.6, id = 12];
//    string attribute13 [default = "Hello World", id = 13];
//    bytes attribute14 [default = "Hello Galaxy", id = 14];
//}

message example.MyTestMessage1 [id = 30001] {
    bool field1 [id = 1];
    char field2 [id = 2];
    int8 field3 [id = 3];
    uint8 field4 [id = 4];
    int16 field5 [id = 5];
    uint16 field6 [id = 6];
    int32 field7 [id = 7];
    uint32 field8 [id = 8];
    int64 field9 [id = 9];
    uint64 field10 [id = 10];
    float field11 [id = 11];
    double field12 [id = 12];
    string field13 [id = 13];
    bytes field14 [id = 14];
}
)";

    testdata::MyTestMessage1 msg1;
    REQUIRE('c' == msg1.attribute2());
    REQUIRE(-1 == msg1.attribute3());
    REQUIRE(2 == msg1.attribute4());
    REQUIRE(-3 == msg1.attribute5());
    REQUIRE(4 == msg1.attribute6());
    REQUIRE(-5 == msg1.attribute7());
    REQUIRE(6 == msg1.attribute8());
    REQUIRE(-7 == msg1.attribute9());
    REQUIRE(8 == msg1.attribute10());
    REQUIRE(-9.5 == Approx(msg1.attribute11()));
    REQUIRE(10.6 == Approx(msg1.attribute12()));
    REQUIRE("Hello World" == msg1.attribute13());
    REQUIRE("Hello Galaxy" == msg1.attribute14());

    cluon::ToProtoVisitor proto;
    msg1.accept(proto);
    std::string s = proto.encodedData();

    std::stringstream sstr{s};
    cluon::FromProtoVisitor protoDecoder;
    protoDecoder.decodeFrom(sstr);

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(msg));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    auto listOfMessages = retVal.first;
    REQUIRE(1 == listOfMessages.size());

    cluon::MetaMessage m = listOfMessages.front();

    cluon::GenericMessage gm;
    gm.createFrom(m, listOfMessages);

    REQUIRE(30001 == gm.ID());
    REQUIRE("MyTestMessage1" == gm.ShortName());
    REQUIRE("example.MyTestMessage1" == gm.LongName());

    // Set values in GenericMessage from ProtoDecoder.
    gm.accept(protoDecoder);

    // Turn GenericMessage back into Proto and decode C++ message.
    {
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

        cluon::ToProtoVisitor proto2;
        gm.accept(proto2);
        std::string s2 = proto2.encodedData();

        std::stringstream sstr2{s2};
        cluon::FromProtoVisitor protoDecoder2;
        protoDecoder2.decodeFrom(sstr2);

        src.accept(protoDecoder2);
        REQUIRE(src.attribute1());
        REQUIRE('c' == src.attribute2());
        REQUIRE(-1 == src.attribute3());
        REQUIRE(2 == src.attribute4());
        REQUIRE(-3 == src.attribute5());
        REQUIRE(4 == src.attribute6());
        REQUIRE(-5 == src.attribute7());
        REQUIRE(6 == src.attribute8());
        REQUIRE(-7 == src.attribute9());
        REQUIRE(8 == src.attribute10());
        REQUIRE(-9.5 == Approx(src.attribute11()));
        REQUIRE(10.6 == Approx(src.attribute12()));
        REQUIRE("Hello World" == src.attribute13());
        REQUIRE("Hello Galaxy" == src.attribute14());
    }
}

TEST_CASE("Dynamically creating messages for nested message.") {
    const char *msg = R"(
package ABC;
message MyTest.TimeStamp [id = 12] {
    int32 seconds      [id = 1];
    int32 microseconds [id = 2];
}

message MyTest.Envelope [id = 1] {
    int32 dataType                      [id = 1];
    bytes serializedData                [id = 2];
    MyTest.TimeStamp sent               [id = 3];
    MyTest.TimeStamp received           [id = 4];
    MyTest.TimeStamp sampleTimeStamp    [id = 5];
    uint32 senderStamp                  [id = 6];
}
)";

    cluon::data::TimeStamp ts1;
    ts1.seconds(1).microseconds(2);
    cluon::data::TimeStamp ts2;
    ts2.seconds(10).microseconds(20);
    cluon::data::TimeStamp ts3;
    ts3.seconds(100).microseconds(200);

    cluon::data::Envelope env;
    env.senderStamp(1).sent(ts1).received(ts2).sampleTimeStamp(ts3).dataType(123).serializedData("ABCDEF");

    REQUIRE("ABCDEF" == env.serializedData());
    REQUIRE(1 == env.senderStamp());
    REQUIRE(123 == env.dataType());
    REQUIRE(1 == env.sent().seconds());
    REQUIRE(2 == env.sent().microseconds());
    REQUIRE(10 == env.received().seconds());
    REQUIRE(20 == env.received().microseconds());
    REQUIRE(100 == env.sampleTimeStamp().seconds());
    REQUIRE(200 == env.sampleTimeStamp().microseconds());

    cluon::ToProtoVisitor proto;
    env.accept(proto);
    std::string s = proto.encodedData();

    std::stringstream sstr{s};
    cluon::FromProtoVisitor protoDecoder;
    protoDecoder.decodeFrom(sstr);

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(msg));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    auto listOfMessages = retVal.first;
    REQUIRE(2 == listOfMessages.size());

    cluon::MetaMessage m = listOfMessages[1];

    cluon::GenericMessage gm;
    gm.createFrom(m, listOfMessages);
    // Set values in GenericMessage from ProtoDecoder.
    gm.accept(protoDecoder);

    // Turn GenericMessage back into Proto and decode C++ message.
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

        cluon::ToProtoVisitor proto2;
        gm.accept(proto2);
        std::string s2 = proto2.encodedData();

        std::stringstream sstr2{s2};
        cluon::FromProtoVisitor protoDecoder2;
        protoDecoder2.decodeFrom(sstr2);

        cluon::FromProtoVisitor protoDecoder3;
        protoDecoder3 = protoDecoder2;

        env2.accept(protoDecoder3);
        REQUIRE(!env2.serializedData().empty());
        REQUIRE("ABCDEF" == env2.serializedData());
        REQUIRE(1 == env2.senderStamp());
        REQUIRE(123 == env2.dataType());
        REQUIRE(1 == env2.sent().seconds());
        REQUIRE(2 == env2.sent().microseconds());
        REQUIRE(10 == env2.received().seconds());
        REQUIRE(20 == env2.received().microseconds());
        REQUIRE(100 == env2.sampleTimeStamp().seconds());
        REQUIRE(200 == env2.sampleTimeStamp().microseconds());
    }
}
