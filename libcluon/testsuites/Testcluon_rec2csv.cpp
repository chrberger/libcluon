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

#include "cluon-rec2csv.hpp"

#include "cluon/Envelope.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/cluonDataStructures.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <fstream>
#include <sstream>
#include <string>

// clang-format off
#ifdef WIN32
    #define UNLINK _unlink
#else
    #include <unistd.h>
    #define UNLINK unlink
#endif
// clang-format on

TEST_CASE("Test empty commandline parameters.") {
    int32_t argc       = 1;
    const char *argv[] = {static_cast<const char *>("cluon-rec2csv")};
    REQUIRE(1 == cluon_rec2csv(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test only --odvd parameter.") {
    UNLINK("ABC1.odvd");
    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-rec2csv"), static_cast<const char *>("--odvd=ABC1.odvd")};
    REQUIRE(1 == cluon_rec2csv(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test only --rec parameter.") {
    UNLINK("DEF2.rec");
    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-rec2csv"), static_cast<const char *>("--rec=DEF2.rec")};
    REQUIRE(1 == cluon_rec2csv(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test both parameters and non-existing ODVD file.") {
    UNLINK("ABC3.odvd");
    UNLINK("DEF3.rec");
    constexpr int32_t argc = 3;
    const char *argv[]
        = {static_cast<const char *>("cluon-rec2csv"), static_cast<const char *>("--odvd=ABC3.odvd"), static_cast<const char *>("--rec=DEF3.rec")};
    std::fstream rec("DEF3.rec", std::ios::out);
    REQUIRE(1 == cluon_rec2csv(argc, const_cast<char **>(argv)));
    UNLINK("DEF3.rec");
}

TEST_CASE("Test both parameters and non-existing rec file.") {
    UNLINK("ABC4.odvd");
    UNLINK("DEF4.rec");
    constexpr int32_t argc = 3;
    const char *argv[]
        = {static_cast<const char *>("cluon-rec2csv"), static_cast<const char *>("--odvd=ABC4.odvd"), static_cast<const char *>("--rec=DEF4.rec")};

    const char *input = R"(
message MyPackage.MyMessage1 [id = 1] {
    string s [id = 1];
}
)";
    std::string messageSpecification(input);

    std::fstream odvd("ABC4.odvd", std::ios::out);
    odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
    odvd.close();

    REQUIRE(1 == cluon_rec2csv(argc, const_cast<char **>(argv)));
    UNLINK("ABC4.odvd");
}

TEST_CASE("Test both parameters.") {
    UNLINK("ABC5.odvd");
    UNLINK("DEF5.rec");
    UNLINK("testdata.MyTestMessage5-0.csv");

    constexpr int32_t argc = 3;
    const char *argv[]
        = {static_cast<const char *>("cluon-rec2csv"), static_cast<const char *>("--odvd=ABC5.odvd"), static_cast<const char *>("--rec=DEF5.rec")};

    const char *input = R"(
message testdata.MyTestMessage5 [id = 30005] {
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
    std::string messageSpecification(input);

    std::fstream odvd("ABC5.odvd", std::ios::out);
    odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
    odvd.close();

    constexpr int32_t MAX_ENTRIES{2};
    {
        std::fstream recordingFile("DEF5.rec", std::ios::out | std::ios::binary | std::ios::trunc);
        REQUIRE(recordingFile.good());

        for (int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter + 1);

            cluon::ToProtoVisitor proto;
            msg.accept(proto);

            cluon::data::Envelope env;
            cluon::data::TimeStamp sent;
            sent.seconds(1000).microseconds(entryCounter);
            cluon::data::TimeStamp received;
            received.seconds(5000).microseconds(entryCounter);
            cluon::data::TimeStamp sampleTimeStamp;
            sampleTimeStamp.seconds(10000).microseconds(entryCounter);

            env.serializedData(proto.encodedData());
            env.dataType(testdata::MyTestMessage5::ID()).sent(sent).received(received).sampleTimeStamp(sampleTimeStamp);

            const std::string tmp{cluon::serializeEnvelope(std::move(env))};
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }

    REQUIRE(0 == cluon_rec2csv(argc, const_cast<char **>(argv)));

    std::fstream CSV("testdata.MyTestMessage5-0.csv", std::ios::in);
    REQUIRE(CSV.good());

    std::stringstream sstrOutput;
    std::string output;
    while (getline(CSV, output)) { sstrOutput << output << std::endl; }
    CSV.close();
    output = sstrOutput.str();

    const char *expectedOutput
        = R"(sent.seconds;sent.microseconds;received.seconds;received.microseconds;sampleTimeStamp.seconds;sampleTimeStamp.microseconds;attribute1;attribute2;attribute3;attribute4;attribute5;attribute6;attribute7;attribute8;attribute9;attribute10;attribute11;
1000;0;5000;0;10000;0;1;-1;100;-100;10000;1;12345;-12345;-1.2345;-10.2345;"Hello World!";
1000;1;5000;1;10000;1;1;-1;100;-100;10000;2;12345;-12345;-1.2345;-10.2345;"Hello World!";
)";

    REQUIRE(output == std::string(expectedOutput));

    UNLINK("ABC5.odvd");
    UNLINK("DEF5.rec");
    UNLINK("testdata.MyTestMessage5-0.csv");
}
