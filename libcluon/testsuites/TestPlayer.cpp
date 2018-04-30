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

#include "cluon/Envelope.hpp"
#include "cluon/Player.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/cluonDataStructures.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>

TEST_CASE("Create simple player for non existing file.") {
    constexpr bool AUTO_REWIND{false};
    constexpr bool THREADING{false};

    {
        std::fstream fileDoesNotExist("/pmt/this/file/does/not/exist", std::ios::in);
        REQUIRE(!fileDoesNotExist.good());
    }
    cluon::Player player("/pmt/this/file/does/not/exist", AUTO_REWIND, THREADING);

    REQUIRE(!player.hasMoreData());
    REQUIRE(0 == player.totalNumberOfEnvelopesInRecFile());
    REQUIRE(!player.getNextEnvelopeToBeReplayed().first);
    REQUIRE(0 == player.delay());
}

TEST_CASE("Create simple player for file with one entry.") {
    constexpr bool AUTO_REWIND{false};
    constexpr bool THREADING{false};

    constexpr uint32_t MAX_ENTRIES{1};
    {
        std::fstream recordingFile("rec1", std::ios::out|std::ios::binary|std::ios::trunc);
        REQUIRE(recordingFile.good());

        for(uint32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute5(entryCounter+1);

            cluon::ToProtoVisitor proto;
            msg.accept(proto);

            cluon::data::Envelope env;
            cluon::data::TimeStamp sent; sent.seconds(1000).microseconds(entryCounter);
            cluon::data::TimeStamp received; received.seconds(5000).microseconds(entryCounter);
            cluon::data::TimeStamp sampleTimeStamp; sampleTimeStamp.seconds(10000).microseconds(entryCounter);

            env.serializedData(proto.encodedData());
            env.dataType(testdata::MyTestMessage5::ID())
               .sent(sent)
               .received(received)
               .sampleTimeStamp(sampleTimeStamp);

            const std::string tmp{cluon::serializeEnvelope(std::move(env))};
            recordingFile.write(tmp.c_str(), tmp.size());
            recordingFile.flush();
        }
        recordingFile.close();
    }
    cluon::Player player("rec1", AUTO_REWIND, THREADING);

    REQUIRE(player.hasMoreData());
    REQUIRE(1 == player.totalNumberOfEnvelopesInRecFile());

    uint32_t retrievedEntries{0};
    while(player.hasMoreData()) {
        auto entry = player.getNextEnvelopeToBeReplayed();
        REQUIRE(entry.first);

        cluon::data::Envelope env = entry.second;
        REQUIRE(testdata::MyTestMessage5::ID() == env.dataType());

        REQUIRE(1000 == env.sent().seconds());
        REQUIRE(retrievedEntries == env.sent().microseconds());
        REQUIRE(5000 == env.received().seconds());
        REQUIRE(retrievedEntries == env.received().microseconds());
        REQUIRE(10000 == env.sampleTimeStamp().seconds());
        REQUIRE(retrievedEntries == env.sampleTimeStamp().microseconds());

        retrievedEntries++;

        testdata::MyTestMessage5 msg = cluon::extractMessage<testdata::MyTestMessage5>(std::move(env));
        REQUIRE(retrievedEntries == msg.attribute5());
    }
    REQUIRE(MAX_ENTRIES == retrievedEntries);
}

