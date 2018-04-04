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

#include "cluon/Envelope.hpp"
#include "cluon/EnvelopeConverter.hpp"
#include "cluon/FromJSONVisitor.hpp"
#include "cluon/FromProtoVisitor.hpp"
#include "cluon/ToJSONVisitor.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"
#include "cluon/cluonTestDataStructures.hpp"

TEST_CASE("Transform Envelope into JSON represention without message specification.") {
    cluon::EnvelopeConverter envConverter;
    REQUIRE("{}" == envConverter.getJSONFromProtoEncodedEnvelope("Hello World"));
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

    // Next, turn Envelope into Proto-encoded byte stream.
    std::string envelopeAsProto;
    {
        cluon::ToProtoVisitor proto;
        env.accept(proto);
        envelopeAsProto = proto.encodedData();
    }
    REQUIRE(30 == envelopeAsProto.size());

    REQUIRE(0x8 == static_cast<uint8_t>(envelopeAsProto.at(0)));
    REQUIRE(0x18 == static_cast<uint8_t>(envelopeAsProto.at(1)));
    REQUIRE(0x12 == static_cast<uint8_t>(envelopeAsProto.at(2)));
    REQUIRE(0x4 == static_cast<uint8_t>(envelopeAsProto.at(3)));
    REQUIRE(0x8 == static_cast<uint8_t>(envelopeAsProto.at(4)));
    REQUIRE(0x6 == static_cast<uint8_t>(envelopeAsProto.at(5)));
    REQUIRE(0x10 == static_cast<uint8_t>(envelopeAsProto.at(6)));
    REQUIRE(0x8 == static_cast<uint8_t>(envelopeAsProto.at(7)));
    REQUIRE(0x1a == static_cast<uint8_t>(envelopeAsProto.at(8)));
    REQUIRE(0x4 == static_cast<uint8_t>(envelopeAsProto.at(9)));
    REQUIRE(0x8 == static_cast<uint8_t>(envelopeAsProto.at(10)));
    REQUIRE(0x2 == static_cast<uint8_t>(envelopeAsProto.at(11)));
    REQUIRE(0x10 == static_cast<uint8_t>(envelopeAsProto.at(12)));
    REQUIRE(0x4 == static_cast<uint8_t>(envelopeAsProto.at(13)));
    REQUIRE(0x22 == static_cast<uint8_t>(envelopeAsProto.at(14)));
    REQUIRE(0x4 == static_cast<uint8_t>(envelopeAsProto.at(15)));
    REQUIRE(0x8 == static_cast<uint8_t>(envelopeAsProto.at(16)));
    REQUIRE(0x14 == static_cast<uint8_t>(envelopeAsProto.at(17)));
    REQUIRE(0x10 == static_cast<uint8_t>(envelopeAsProto.at(18)));
    REQUIRE(0x28 == static_cast<uint8_t>(envelopeAsProto.at(19)));
    REQUIRE(0x2a == static_cast<uint8_t>(envelopeAsProto.at(20)));
    REQUIRE(0x6 == static_cast<uint8_t>(envelopeAsProto.at(21)));
    REQUIRE(0x8 == static_cast<uint8_t>(envelopeAsProto.at(22)));
    REQUIRE(0xc8 == static_cast<uint8_t>(envelopeAsProto.at(23)));
    REQUIRE(0x1 == static_cast<uint8_t>(envelopeAsProto.at(24)));
    REQUIRE(0x10 == static_cast<uint8_t>(envelopeAsProto.at(25)));
    REQUIRE(0x90 == static_cast<uint8_t>(envelopeAsProto.at(26)));
    REQUIRE(0x3 == static_cast<uint8_t>(envelopeAsProto.at(27)));
    REQUIRE(0x30 == static_cast<uint8_t>(envelopeAsProto.at(28)));
    REQUIRE(0x2 == static_cast<uint8_t>(envelopeAsProto.at(29)));

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

        std::stringstream sstr{envelopeAsProto};
        cluon::FromProtoVisitor protoDecoder;
        protoDecoder.decodeFrom(sstr);

        env2.accept(protoDecoder);
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

    uint32_t length = static_cast<uint32_t>(envelopeAsProto.size()); // NOLINT
    length          = htole32(length);
    length <<= 8;
    length |= 0xA4;

    std::stringstream sstr;
    constexpr char c{0x0D};
    sstr.write(&c, sizeof(char));
    sstr.write(reinterpret_cast<const char *>(&length), sizeof(uint32_t)); // NOLINT
    sstr.write(&envelopeAsProto[0], static_cast<std::streamsize>(envelopeAsProto.size()));

    const std::string output{sstr.str()};

    REQUIRE(0x0d == static_cast<uint8_t>(output.at(0)));
    REQUIRE(0xa4 == static_cast<uint8_t>(output.at(1)));
    REQUIRE(0x1e == static_cast<uint8_t>(output.at(2)));
    REQUIRE(0x0 == static_cast<uint8_t>(output.at(3)));
    REQUIRE(0x0 == static_cast<uint8_t>(output.at(4)));
    REQUIRE(0x8 == static_cast<uint8_t>(output.at(5)));
    REQUIRE(0x18 == static_cast<uint8_t>(output.at(6)));
    REQUIRE(0x12 == static_cast<uint8_t>(output.at(7)));
    REQUIRE(0x4 == static_cast<uint8_t>(output.at(8)));
    REQUIRE(0x8 == static_cast<uint8_t>(output.at(9)));
    REQUIRE(0x6 == static_cast<uint8_t>(output.at(10)));
    REQUIRE(0x10 == static_cast<uint8_t>(output.at(11)));
    REQUIRE(0x8 == static_cast<uint8_t>(output.at(12)));
    REQUIRE(0x1a == static_cast<uint8_t>(output.at(13)));
    REQUIRE(0x4 == static_cast<uint8_t>(output.at(14)));
    REQUIRE(0x8 == static_cast<uint8_t>(output.at(15)));
    REQUIRE(0x2 == static_cast<uint8_t>(output.at(16)));
    REQUIRE(0x10 == static_cast<uint8_t>(output.at(17)));
    REQUIRE(0x4 == static_cast<uint8_t>(output.at(18)));
    REQUIRE(0x22 == static_cast<uint8_t>(output.at(19)));
    REQUIRE(0x4 == static_cast<uint8_t>(output.at(20)));
    REQUIRE(0x8 == static_cast<uint8_t>(output.at(21)));
    REQUIRE(0x14 == static_cast<uint8_t>(output.at(22)));
    REQUIRE(0x10 == static_cast<uint8_t>(output.at(23)));
    REQUIRE(0x28 == static_cast<uint8_t>(output.at(24)));
    REQUIRE(0x2a == static_cast<uint8_t>(output.at(25)));
    REQUIRE(0x6 == static_cast<uint8_t>(output.at(26)));
    REQUIRE(0x8 == static_cast<uint8_t>(output.at(27)));
    REQUIRE(0xc8 == static_cast<uint8_t>(output.at(28)));
    REQUIRE(0x1 == static_cast<uint8_t>(output.at(29)));
    REQUIRE(0x10 == static_cast<uint8_t>(output.at(30)));
    REQUIRE(0x90 == static_cast<uint8_t>(output.at(31)));
    REQUIRE(0x3 == static_cast<uint8_t>(output.at(32)));
    REQUIRE(0x30 == static_cast<uint8_t>(output.at(33)));
    REQUIRE(0x2 == static_cast<uint8_t>(output.at(34)));

    // output contains now an Envelope in a structure similar to how
    // a Container would be encoded.

    const char *messageSpecification = R"(
message example.TimeStamp [id = 12] {
    int32 seconds      [id = 1];
    int32 microseconds [id = 2];
}

message example.Envelope [id = 1] {
    int32 dataType                      [id = 1];
    bytes serializedData                [id = 2];
    example.TimeStamp sent              [id = 3];
    example.TimeStamp received          [id = 4];
    example.TimeStamp sampleTimeStamp   [id = 5];
    uint32 senderStamp                  [id = 6];
}
)";

    const char *JSON = R"({"dataType":12,
"sent":{"seconds":1,
"microseconds":2},
"received":{"seconds":10,
"microseconds":20},
"sampleTimeStamp":{"seconds":100,
"microseconds":200},
"senderStamp":2,
"example_TimeStamp":{"seconds":3,
"microseconds":4}})";

    cluon::EnvelopeConverter envConverter;
    REQUIRE(2 == envConverter.setMessageSpecification(std::string(messageSpecification)));

    // Test with Envelope:
    const std::string JSON_A = envConverter.getJSONFromEnvelope(env);
    REQUIRE(std::string(JSON) == JSON_A);

    // Test without OD4 header:
    const std::string JSON_B = envConverter.getJSONFromProtoEncodedEnvelope(envelopeAsProto);
    REQUIRE(std::string(JSON) == JSON_B);

    // Test with OD4-header:
    const std::string JSON_C = envConverter.getJSONFromProtoEncodedEnvelope(output);
    REQUIRE(std::string(JSON) == JSON_C);
}

TEST_CASE("Transform Envelope into JSON represention for complex payload.") {
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

    env.senderStamp(2).sent(ts1).received(ts2).sampleTimeStamp(ts3).dataType(30001);
    {
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
        env.serializedData(proto.encodedData());
    }

    REQUIRE(2 == env.senderStamp());
    REQUIRE(30001 == env.dataType());
    REQUIRE(1 == env.sent().seconds());
    REQUIRE(2 == env.sent().microseconds());
    REQUIRE(10 == env.received().seconds());
    REQUIRE(20 == env.received().microseconds());
    REQUIRE(100 == env.sampleTimeStamp().seconds());
    REQUIRE(200 == env.sampleTimeStamp().microseconds());

    REQUIRE(61 == env.serializedData().size());

    // Next, turn Envelope into Proto-encoded byte stream.
    std::string envelopeAsProto;
    {
        cluon::ToProtoVisitor proto;
        env.accept(proto);
        envelopeAsProto = proto.encodedData();
    }

    uint32_t length = static_cast<uint32_t>(envelopeAsProto.size()); // NOLINT
    length          = htole32(length);
    length <<= 8;
    length |= 0xA4;

    std::stringstream sstr;
    constexpr char c{0x0D};
    sstr.write(&c, sizeof(char));
    sstr.write(reinterpret_cast<const char *>(&length), sizeof(uint32_t)); // NOLINT
    sstr.write(&envelopeAsProto[0], static_cast<std::streamsize>(envelopeAsProto.size()));

    const std::string output{sstr.str()};

    // output contains now an Envelope in a structure similar to how
    // a Container would be encoded.

    const char *messageSpecification = R"(
message TestMessageA [id = 30001] {
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

    const char *JSON = R"({"dataType":30001,
"sent":{"seconds":1,
"microseconds":2},
"received":{"seconds":10,
"microseconds":20},
"sampleTimeStamp":{"seconds":100,
"microseconds":200},
"senderStamp":2,
"TestMessageA":{"attribute1":1,
"attribute2":"c",
"attribute3":-1,
"attribute4":2,
"attribute5":-3,
"attribute6":4,
"attribute7":-5,
"attribute8":6,
"attribute9":-7,
"attribute10":8,
"attribute11":-9.5,
"attribute12":10.6,
"attribute13":"SGVsbG8gV29ybGQ=",
"attribute14":"SGVsbG8gR2FsYXh5"}})";

    cluon::EnvelopeConverter envConverter;
    REQUIRE(1 == envConverter.setMessageSpecification(std::string(messageSpecification)));

    // Test with Envelope:
    const std::string JSON_A = envConverter.getJSONFromEnvelope(env);
    REQUIRE(std::string(JSON) == JSON_A);

    // Test without OD4 header:
    const std::string JSON_B = envConverter.getJSONFromProtoEncodedEnvelope(envelopeAsProto);
    REQUIRE(std::string(JSON) == JSON_B);

    // Test with OD4-header:
    const std::string JSON_C = envConverter.getJSONFromProtoEncodedEnvelope(output);
    REQUIRE(std::string(JSON) == JSON_C);
}

TEST_CASE("Transform Envelope into JSON represention for nested payloads.") {
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

    env.senderStamp(2).sent(ts1).received(ts2).sampleTimeStamp(ts3).dataType(30006);
    {
        testdata::MyTestMessage2 tm2;
        tm2.attribute1(99);

        testdata::MyTestMessage6 tm6;
        tm6.attribute1(tm2);

        cluon::ToProtoVisitor proto;
        tm6.accept(proto);
        env.serializedData(proto.encodedData());
    }

    REQUIRE(2 == env.senderStamp());
    REQUIRE(30006 == env.dataType());
    REQUIRE(1 == env.sent().seconds());
    REQUIRE(2 == env.sent().microseconds());
    REQUIRE(10 == env.received().seconds());
    REQUIRE(20 == env.received().microseconds());
    REQUIRE(100 == env.sampleTimeStamp().seconds());
    REQUIRE(200 == env.sampleTimeStamp().microseconds());

    REQUIRE(4 == env.serializedData().size());

    // Next, turn Envelope into Proto-encoded byte stream.
    std::string envelopeAsProto;
    {
        cluon::ToProtoVisitor proto;
        env.accept(proto);
        envelopeAsProto = proto.encodedData();
    }

    uint32_t length = static_cast<uint32_t>(envelopeAsProto.size()); // NOLINT
    length          = htole32(length);
    length <<= 8;
    length |= 0xA4;

    std::stringstream sstr;
    constexpr char c{0x0D};
    sstr.write(&c, sizeof(char));
    sstr.write(reinterpret_cast<const char *>(&length), sizeof(uint32_t)); // NOLINT
    sstr.write(&envelopeAsProto[0], static_cast<std::streamsize>(envelopeAsProto.size()));

    const std::string output{sstr.str()};

    // output contains now an Envelope in a structure similar to how
    // a Container would be encoded.

    const char *messageSpecification = R"(
message MessageA [id = 30002] {
    uint8 attribute1 [ default = 123, id = 1 ];
}

message MessageB [id = 30006] {
    MessageA attribute1 [ id = 3 ];
}
)";

    const char *JSON = R"({"dataType":30006,
"sent":{"seconds":1,
"microseconds":2},
"received":{"seconds":10,
"microseconds":20},
"sampleTimeStamp":{"seconds":100,
"microseconds":200},
"senderStamp":2,
"MessageB":{"attribute1":{"attribute1":99}}})";

    cluon::EnvelopeConverter envConverter;
    REQUIRE(2 == envConverter.setMessageSpecification(std::string(messageSpecification)));

    // Test with Envelope:
    const std::string JSON_A = envConverter.getJSONFromEnvelope(env);
    REQUIRE(std::string(JSON) == JSON_A);

    // Test without OD4 header:
    const std::string JSON_B = envConverter.getJSONFromProtoEncodedEnvelope(envelopeAsProto);
    REQUIRE(std::string(JSON) == JSON_B);

    // Test with OD4-header:
    const std::string JSON_C = envConverter.getJSONFromProtoEncodedEnvelope(output);
    REQUIRE(std::string(JSON) == JSON_C);
}

TEST_CASE("Transform Envelope into JSON represention for nested payloads with punctuated type names to be converted to "
          "underscores.") {
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

    env.senderStamp(2).sent(ts1).received(ts2).sampleTimeStamp(ts3).dataType(30006);
    {
        testdata::MyTestMessage2 tm2;
        tm2.attribute1(99);

        testdata::MyTestMessage6 tm6;
        tm6.attribute1(tm2);

        cluon::ToProtoVisitor proto;
        tm6.accept(proto);
        env.serializedData(proto.encodedData());
    }

    REQUIRE(2 == env.senderStamp());
    REQUIRE(30006 == env.dataType());
    REQUIRE(1 == env.sent().seconds());
    REQUIRE(2 == env.sent().microseconds());
    REQUIRE(10 == env.received().seconds());
    REQUIRE(20 == env.received().microseconds());
    REQUIRE(100 == env.sampleTimeStamp().seconds());
    REQUIRE(200 == env.sampleTimeStamp().microseconds());

    REQUIRE(4 == env.serializedData().size());

    // Next, turn Envelope into Proto-encoded byte stream.
    std::string envelopeAsProto;
    {
        cluon::ToProtoVisitor proto;
        env.accept(proto);
        envelopeAsProto = proto.encodedData();
    }

    uint32_t length = static_cast<uint32_t>(envelopeAsProto.size()); // NOLINT
    length          = htole32(length);
    length <<= 8;
    length |= 0xA4;

    std::stringstream sstr;
    constexpr char c{0x0D};
    sstr.write(&c, sizeof(char));
    sstr.write(reinterpret_cast<const char *>(&length), sizeof(uint32_t)); // NOLINT
    sstr.write(&envelopeAsProto[0], static_cast<std::streamsize>(envelopeAsProto.size()));

    const std::string output{sstr.str()};

    // output contains now an Envelope in a structure similar to how
    // a Container would be encoded.

    const char *messageSpecification = R"(
package ABC.DEF;
message GHI.MessageA [id = 30002] {
    uint8 attribute1 [ default = 123, id = 1 ];
}

message GHI.MessageB [id = 30006] {
    GHI.MessageA attribute1 [ id = 3 ];
}
)";

    const char *JSON = R"({"dataType":30006,
"sent":{"seconds":1,
"microseconds":2},
"received":{"seconds":10,
"microseconds":20},
"sampleTimeStamp":{"seconds":100,
"microseconds":200},
"senderStamp":2,
"GHI_MessageB":{"attribute1":{"attribute1":99}}})";

    cluon::EnvelopeConverter envConverter;
    REQUIRE(2 == envConverter.setMessageSpecification(std::string(messageSpecification)));

    // Test with Envelope:
    const std::string JSON_A = envConverter.getJSONFromEnvelope(env);
    REQUIRE(std::string(JSON) == JSON_A);

    // Test without OD4 header:
    const std::string JSON_B = envConverter.getJSONFromProtoEncodedEnvelope(envelopeAsProto);
    REQUIRE(std::string(JSON) == JSON_B);

    // Test with OD4-header:
    const std::string JSON_C = envConverter.getJSONFromProtoEncodedEnvelope(output);
    REQUIRE(std::string(JSON) == JSON_C);
}

TEST_CASE("Convert JSON representation of MyTestMessage5 into Proto-encoded data and decode Message again.") {
    // First, prepare message to encode as JSON.
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

    // Second, set values.
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

    // Third, create JSON representation.
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

    // Fourth, dynamically provide a message specification to create Envelope from JSON.
    const char *messageSpecification = R"(
message MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
    int32 attribute6 [ default = -10000, id = 6 ];
    uint64 attribute7 [ default = 12345, id = 7 ];
    int64 attribute8 [ default = -12345, id = 8 ];
    float attribute9 [ default = -1.2345, id = 9 ];
    double attribute10 [ default = -10.2345, id = 10 ];
    string attribute11 [ default = "Hello World!", id = 11 ];
}
)";

    // Fifth, parse message specification.
    cluon::EnvelopeConverter envConverter;
    REQUIRE(1 == envConverter.setMessageSpecification(std::string(messageSpecification)));

    // Sixth, turn JSON into proper Envelope.
    constexpr int32_t MESSAGE_IDENTIFIER{30005};
    std::string protoEncodedData{envConverter.getProtoEncodedEnvelopeFromJSONWithoutTimeStamps(jsonEncoder.json(), MESSAGE_IDENTIFIER)};
    REQUIRE(!protoEncodedData.empty());

    // Seventh, read back Envelope from serialization.
    std::stringstream sstr(protoEncodedData);
    auto retVal = cluon::extractEnvelope(sstr);

    REQUIRE(retVal.first);
    cluon::data::Envelope env;
    REQUIRE(0 == env.dataType());

    // Verify values in transformed Envelope.
    env = retVal.second;
    REQUIRE(MESSAGE_IDENTIFIER == env.dataType());
    REQUIRE(0 == (env.sent().seconds() + env.sent().microseconds()));
    REQUIRE(0 == (env.received().seconds() + env.received().microseconds()));
    REQUIRE(0 == (env.sampleTimeStamp().seconds() + env.sampleTimeStamp().microseconds()));

    // Eighth, read back message.
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

    tmp2 = cluon::extractMessage<testdata::MyTestMessage5>(std::move(env));

    // Simple toString().
    std::stringstream buffer;
    tmp2.accept([](int32_t, const std::string &, const std::string &) {},
                [&buffer](uint32_t, std::string &&, std::string &&n, auto v) { buffer << n << " = " << v << '\n'; },
                []() {});
    std::cout << buffer.str() << std::endl;

    // Finally, compare decoded values.
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

TEST_CASE("Convert JSON representation of MyTestMessage5 into Proto-encoded data with no message specification.") {
    // First, prepare message to encode as JSON.
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

    // Second, set values.
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

    // Third, create JSON representation.
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

    // Fifth, parse message specification.
    cluon::EnvelopeConverter envConverter;

    // Sixth, turn JSON into proper Envelope.
    constexpr int32_t MESSAGE_IDENTIFIER{30005};
    std::string protoEncodedData{envConverter.getProtoEncodedEnvelopeFromJSONWithoutTimeStamps(jsonEncoder.json(), MESSAGE_IDENTIFIER)};
    REQUIRE(protoEncodedData.empty());

    // Seventh, read back Envelope from serialization.
    std::stringstream sstr(protoEncodedData);
    auto retVal = cluon::extractEnvelope(sstr);

    REQUIRE(!retVal.first);
    cluon::data::Envelope env;
    REQUIRE(0 == env.dataType());

    // Verify values in transformed Envelope.
    env = retVal.second;
    REQUIRE(MESSAGE_IDENTIFIER != env.dataType());
}

TEST_CASE("Convert JSON representation of MyTestMessage5 into Proto-encoded data with wrong message identifier.") {
    // First, prepare message to encode as JSON.
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

    // Second, set values.
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

    // Third, create JSON representation.
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

    // Fourth, dynamically provide a message specification to create Envelope from JSON.
    const char *messageSpecification = R"(
message MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
    int32 attribute6 [ default = -10000, id = 6 ];
    uint64 attribute7 [ default = 12345, id = 7 ];
    int64 attribute8 [ default = -12345, id = 8 ];
    float attribute9 [ default = -1.2345, id = 9 ];
    double attribute10 [ default = -10.2345, id = 10 ];
    string attribute11 [ default = "Hello World!", id = 11 ];
}
)";

    // Fifth, parse message specification.
    cluon::EnvelopeConverter envConverter;
    REQUIRE(1 == envConverter.setMessageSpecification(std::string(messageSpecification)));

    // Sixth, turn JSON into proper Envelope.
    constexpr int32_t MESSAGE_IDENTIFIER{30005 + 1};
    std::string protoEncodedData{envConverter.getProtoEncodedEnvelopeFromJSONWithoutTimeStamps(jsonEncoder.json(), MESSAGE_IDENTIFIER)};
    REQUIRE(protoEncodedData.empty());

    // Seventh, read back Envelope from serialization.
    std::stringstream sstr(protoEncodedData);
    auto retVal = cluon::extractEnvelope(sstr);

    REQUIRE(!retVal.first);
    cluon::data::Envelope env;
    REQUIRE(0 == env.dataType());

    // Verify values in transformed Envelope.
    env = retVal.second;
    REQUIRE(MESSAGE_IDENTIFIER != env.dataType());
    REQUIRE(0 == (env.sent().seconds() + env.sent().microseconds()));
    REQUIRE(0 == (env.received().seconds() + env.received().microseconds()));
    REQUIRE(0 == (env.sampleTimeStamp().seconds() + env.sampleTimeStamp().microseconds()));
}

TEST_CASE("Convert JSON representation of MyTestMessage5 into Proto-encoded data with junk data.") {
    // Fourth, dynamically provide a message specification to create Envelope from JSON.
    const char *messageSpecification = R"(
message MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
    int32 attribute6 [ default = -10000, id = 6 ];
    uint64 attribute7 [ default = 12345, id = 7 ];
    int64 attribute8 [ default = -12345, id = 8 ];
    float attribute9 [ default = -1.2345, id = 9 ];
    double attribute10 [ default = -10.2345, id = 10 ];
    string attribute11 [ default = "Hello World!", id = 11 ];
}
)";

    // Fifth, parse message specification.
    cluon::EnvelopeConverter envConverter;
    REQUIRE(1 == envConverter.setMessageSpecification(std::string(messageSpecification)));

    std::string JUNK
        = "Gallia est omnis divisa in partes tres, quarum unam incolunt Belgae, aliam Aquitani, tertiam qui ipsorum lingua Celtae, nostra Galli appellantur. "
          "Hi omnes lingua, institutis, legibus inter se differunt. Gallos ab Aquitanis Garumna flumen, a Belgis Matrona et Sequana dividit."; // Written by
                                                                                                                                               // Caesar :-D

    // Sixth, turn JSON into proper Envelope.
    constexpr int32_t MESSAGE_IDENTIFIER{30005};
    std::string protoEncodedData{envConverter.getProtoEncodedEnvelopeFromJSONWithoutTimeStamps(JUNK, MESSAGE_IDENTIFIER)};
    REQUIRE(!protoEncodedData.empty());

    // Seventh, read back Envelope from serialization.
    std::stringstream sstr(protoEncodedData);
    auto retVal = cluon::extractEnvelope(sstr);

    REQUIRE(retVal.first);
    cluon::data::Envelope env;
    REQUIRE(0 == env.dataType());

    // Verify values in transformed Envelope.
    env = retVal.second;
    REQUIRE(MESSAGE_IDENTIFIER == env.dataType());
    REQUIRE(0 == (env.sent().seconds() + env.sent().microseconds()));
    REQUIRE(0 == (env.received().seconds() + env.received().microseconds()));
    REQUIRE(0 == (env.sampleTimeStamp().seconds() + env.sampleTimeStamp().microseconds()));

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

    tmp2 = cluon::extractMessage<testdata::MyTestMessage5>(std::move(env));

    // Simple toString().
    std::stringstream buffer;
    tmp2.accept([](int32_t, const std::string &, const std::string &) {},
                [&buffer](uint32_t, std::string &&, std::string &&n, auto v) { buffer << n << " = " << v << '\n'; },
                []() {});
    std::cout << buffer.str() << std::endl;

    // Junk data results in default values.
    REQUIRE(tmp2.attribute1() == 0);
    REQUIRE(tmp2.attribute2() == 0);
    REQUIRE(tmp2.attribute3() == 0);
    REQUIRE(tmp2.attribute4() == 0);
    REQUIRE(tmp2.attribute5() == 0);
    REQUIRE(tmp2.attribute6() == 0);
    REQUIRE(tmp2.attribute7() == 0);
    REQUIRE(tmp2.attribute8() == 0);
    REQUIRE(0 == Approx(tmp2.attribute9()));
    REQUIRE(0 == Approx(tmp2.attribute10()));
    REQUIRE(tmp2.attribute11().empty());
}
