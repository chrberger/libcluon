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

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

// clang-format off
#ifdef WIN32
    #define UNLINK _unlink
#else
    #include <unistd.h>
    #define UNLINK unlink
#endif
// clang-format on


TEST_CASE("Create simple player for non existing file.") {
    constexpr bool AUTO_REWIND{false};
    constexpr bool THREADING{false};

    UNLINK("/pmt/this/file/does/not/exist");
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

    UNLINK("rec1");
    constexpr int32_t MAX_ENTRIES{1};
    {
        std::fstream recordingFile("rec1", std::ios::out|std::ios::binary|std::ios::trunc);
        REQUIRE(recordingFile.good());

        for(int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter+1);

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
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }
    cluon::Player player("rec1", AUTO_REWIND, THREADING);

    REQUIRE(player.hasMoreData());
    REQUIRE(MAX_ENTRIES == player.totalNumberOfEnvelopesInRecFile());

    int32_t retrievedEntries{0};
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
        REQUIRE(retrievedEntries == msg.attribute6());

        REQUIRE(0 == player.delay());
    }
    REQUIRE(MAX_ENTRIES == retrievedEntries);
    UNLINK("rec1");
}

TEST_CASE("Create simple player for file with two entries.") {
    constexpr bool AUTO_REWIND{false};
    constexpr bool THREADING{false};

    UNLINK("rec2");
    constexpr int32_t MAX_ENTRIES{2};
    {
        std::fstream recordingFile("rec2", std::ios::out|std::ios::binary|std::ios::trunc);
        REQUIRE(recordingFile.good());

        for(int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter+1);

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
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }
    cluon::Player player("rec2", AUTO_REWIND, THREADING);

    REQUIRE(player.hasMoreData());
    REQUIRE(MAX_ENTRIES == player.totalNumberOfEnvelopesInRecFile());

    int32_t retrievedEntries{0};
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
        REQUIRE(retrievedEntries == msg.attribute6());

        if (1 == retrievedEntries) {
            REQUIRE(0 == player.delay());
        }
        else {
            REQUIRE(1 == player.delay());
        }
    }
    REQUIRE(MAX_ENTRIES == retrievedEntries);
    UNLINK("rec2");
}

TEST_CASE("Create simple player for file with three entries.") {
    constexpr bool AUTO_REWIND{false};
    constexpr bool THREADING{false};

    UNLINK("rec3");
    constexpr int32_t MAX_ENTRIES{3};
    {
        std::fstream recordingFile("rec3", std::ios::out|std::ios::binary|std::ios::trunc);
        REQUIRE(recordingFile.good());

        for(int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter+1);

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
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }

    cluon::Player player("rec3", AUTO_REWIND, THREADING);

    REQUIRE(player.hasMoreData());
    REQUIRE(MAX_ENTRIES == player.totalNumberOfEnvelopesInRecFile());

    int32_t retrievedEntries{0};
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
        REQUIRE(retrievedEntries == msg.attribute6());

        if (1 == retrievedEntries) {
            REQUIRE(0 == player.delay());
        }
        else {
            REQUIRE(1 == player.delay());
        }
    }
    REQUIRE(MAX_ENTRIES == retrievedEntries);
    UNLINK("rec3");
}

TEST_CASE("Create simple player for file with three entries auto auto-rewind.") {
    constexpr bool AUTO_REWIND{true};
    constexpr bool THREADING{false};

    UNLINK("rec4");
    constexpr int32_t MAX_ENTRIES{3};
    {
        std::fstream recordingFile("rec4", std::ios::out|std::ios::binary|std::ios::trunc);
        REQUIRE(recordingFile.good());

        for(int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter+1);

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
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }
    cluon::Player player("rec4", AUTO_REWIND, THREADING);

    REQUIRE(player.hasMoreData());
    REQUIRE(MAX_ENTRIES == player.totalNumberOfEnvelopesInRecFile());

    int32_t retrievedEntries{0};
    while(player.hasMoreData()) {
        auto entry = player.getNextEnvelopeToBeReplayed();
        REQUIRE(entry.first);

        cluon::data::Envelope env = entry.second;
        REQUIRE(testdata::MyTestMessage5::ID() == env.dataType());

        REQUIRE(1000 == env.sent().seconds());
        REQUIRE(retrievedEntries%3 == env.sent().microseconds());
        REQUIRE(5000 == env.received().seconds());
        REQUIRE(retrievedEntries%3 == env.received().microseconds());
        REQUIRE(10000 == env.sampleTimeStamp().seconds());
        REQUIRE(retrievedEntries%3 == env.sampleTimeStamp().microseconds());

        retrievedEntries++;

        testdata::MyTestMessage5 msg = cluon::extractMessage<testdata::MyTestMessage5>(std::move(env));
        REQUIRE(((retrievedEntries-1)%3)+1 == msg.attribute6());

        if (1 == ((retrievedEntries-1)%3)+1) {
            REQUIRE(0 == player.delay());
        }
        else {
            REQUIRE(1 == player.delay());
        }

        if(10 < retrievedEntries) break;
    }
    REQUIRE(11 == retrievedEntries);
    UNLINK("rec4");
}

TEST_CASE("Create simple player for file with three entries with manual rewind.") {
    constexpr bool AUTO_REWIND{false};
    constexpr bool THREADING{false};

    UNLINK("rec5");
    constexpr int32_t MAX_ENTRIES{3};
    {
        std::fstream recordingFile("rec5", std::ios::out|std::ios::binary|std::ios::trunc);
        REQUIRE(recordingFile.good());

        for(int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter+1);

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
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }
    cluon::Player player("rec5", AUTO_REWIND, THREADING);

    REQUIRE(player.hasMoreData());
    REQUIRE(MAX_ENTRIES == player.totalNumberOfEnvelopesInRecFile());

    bool rewinded{false};
    int32_t retrievedEntries{0};
    while(player.hasMoreData()) {
        auto entry = player.getNextEnvelopeToBeReplayed();
        REQUIRE(entry.first);

        cluon::data::Envelope env = entry.second;
        REQUIRE(testdata::MyTestMessage5::ID() == env.dataType());

        REQUIRE(1000 == env.sent().seconds());
        REQUIRE(retrievedEntries%3 == env.sent().microseconds());
        REQUIRE(5000 == env.received().seconds());
        REQUIRE(retrievedEntries%3 == env.received().microseconds());
        REQUIRE(10000 == env.sampleTimeStamp().seconds());
        REQUIRE(retrievedEntries%3 == env.sampleTimeStamp().microseconds());

        retrievedEntries++;

        testdata::MyTestMessage5 msg = cluon::extractMessage<testdata::MyTestMessage5>(std::move(env));
        REQUIRE(((retrievedEntries-1)%3)+1 == msg.attribute6());

        if (1 == ((retrievedEntries-1)%3)+1) {
            REQUIRE(0 == player.delay());
        }
        else {
            REQUIRE(1 == player.delay());
        }

        if (!rewinded && (3 == retrievedEntries)) {
            player.rewind();
            rewinded = true;
        }
    }
    REQUIRE(6 == retrievedEntries);
    UNLINK("rec5");
}

TEST_CASE("Create simple player for file with 6,000 entries to test look-ahead with threading and player listener.") {
    constexpr bool AUTO_REWIND{false};
    constexpr bool THREADING{true};

    UNLINK("rec6");
    constexpr int32_t MAX_ENTRIES{6000};
    {
        std::fstream recordingFile("rec6", std::ios::out|std::ios::binary|std::ios::trunc);
        REQUIRE(recordingFile.good());

        for(int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter+1);

            cluon::ToProtoVisitor proto;
            msg.accept(proto);

            cluon::data::Envelope env;
            cluon::data::TimeStamp sent; sent.seconds(1000).microseconds(entryCounter);
            cluon::data::TimeStamp received; received.seconds(5000).microseconds(entryCounter);
            cluon::data::TimeStamp sampleTimeStamp; sampleTimeStamp.seconds(entryCounter).microseconds(entryCounter);

            env.serializedData(proto.encodedData());
            env.dataType(testdata::MyTestMessage5::ID())
               .sent(sent)
               .received(received)
               .sampleTimeStamp(sampleTimeStamp);

            const std::string tmp{cluon::serializeEnvelope(std::move(env))};
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }
    cluon::Player player("rec6", AUTO_REWIND, THREADING);

    bool calledPlayerListener{false};
    player.setPlayerListener([&calledPlayerListener](cluon::data::PlayerStatus playerStatus){calledPlayerListener = true;});

    REQUIRE(player.hasMoreData());
    REQUIRE(MAX_ENTRIES == player.totalNumberOfEnvelopesInRecFile());

    int32_t retrievedEntries{0};
    while(player.hasMoreData()) {
        auto entry = player.getNextEnvelopeToBeReplayed();
        REQUIRE(entry.first);

        cluon::data::Envelope env = entry.second;
        REQUIRE(testdata::MyTestMessage5::ID() == env.dataType());

        REQUIRE(1000 == env.sent().seconds());
        REQUIRE(retrievedEntries == env.sent().microseconds());
        REQUIRE(5000 == env.received().seconds());
        REQUIRE(retrievedEntries == env.received().microseconds());
        REQUIRE(retrievedEntries == env.sampleTimeStamp().seconds());
        REQUIRE(retrievedEntries == env.sampleTimeStamp().microseconds());

        retrievedEntries++;

        testdata::MyTestMessage5 msg = cluon::extractMessage<testdata::MyTestMessage5>(std::move(env));
        REQUIRE(retrievedEntries == msg.attribute6());

        if (1 == retrievedEntries) {
            REQUIRE(0 == player.delay());
        }
        else {
            REQUIRE(static_cast<int64_t>(1000*1000) == static_cast<int64_t>(player.delay()));
        }

        std::chrono::duration<uint32_t, std::micro> delay{player.delay()/2000};
        std::this_thread::sleep_for(delay);
    }
    REQUIRE(calledPlayerListener);
    REQUIRE(MAX_ENTRIES == retrievedEntries);
    UNLINK("rec6");
}

TEST_CASE("Create simple player for file with three entries with seeking.") {
    constexpr bool AUTO_REWIND{false};
    constexpr bool THREADING{false};

    UNLINK("rec7");
    constexpr int32_t MAX_ENTRIES{3};
    {
        std::fstream recordingFile("rec7", std::ios::out|std::ios::binary|std::ios::trunc);
        REQUIRE(recordingFile.good());

        for(int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter+1);

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
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }
    cluon::Player player("rec7", AUTO_REWIND, THREADING);

    REQUIRE(player.hasMoreData());
    REQUIRE(MAX_ENTRIES == player.totalNumberOfEnvelopesInRecFile());

    player.seekTo(0.5f);
    REQUIRE(player.hasMoreData());
    {
        auto entry = player.getNextEnvelopeToBeReplayed();
        REQUIRE(entry.first);

        cluon::data::Envelope env = entry.second;
        REQUIRE(testdata::MyTestMessage5::ID() == env.dataType());

        REQUIRE(1000 == env.sent().seconds());
        REQUIRE(1 == env.sent().microseconds());
        REQUIRE(5000 == env.received().seconds());
        REQUIRE(1 == env.received().microseconds());
        REQUIRE(10000 == env.sampleTimeStamp().seconds());
        REQUIRE(1 == env.sampleTimeStamp().microseconds());
        REQUIRE(player.hasMoreData());
    }

    player.seekTo(0);
    REQUIRE(player.hasMoreData());
    {
        auto entry = player.getNextEnvelopeToBeReplayed();
        REQUIRE(entry.first);

        cluon::data::Envelope env = entry.second;
        REQUIRE(testdata::MyTestMessage5::ID() == env.dataType());

        REQUIRE(1000 == env.sent().seconds());
        REQUIRE(0 == env.sent().microseconds());
        REQUIRE(5000 == env.received().seconds());
        REQUIRE(0 == env.received().microseconds());
        REQUIRE(10000 == env.sampleTimeStamp().seconds());
        REQUIRE(0 == env.sampleTimeStamp().microseconds());
        REQUIRE(player.hasMoreData());
    }

    player.seekTo(1.0f);
    REQUIRE(player.hasMoreData());
    {
        auto entry = player.getNextEnvelopeToBeReplayed();
        REQUIRE(entry.first);

        cluon::data::Envelope env = entry.second;
        REQUIRE(testdata::MyTestMessage5::ID() == env.dataType());

        REQUIRE(1000 == env.sent().seconds());
        REQUIRE(2 == env.sent().microseconds());
        REQUIRE(5000 == env.received().seconds());
        REQUIRE(2 == env.received().microseconds());
        REQUIRE(10000 == env.sampleTimeStamp().seconds());
        REQUIRE(2 == env.sampleTimeStamp().microseconds());
        REQUIRE(!player.hasMoreData());
    }

    player.seekTo(0.5f);
    REQUIRE(player.hasMoreData());
    {
        auto entry = player.getNextEnvelopeToBeReplayed();
        REQUIRE(entry.first);

        cluon::data::Envelope env = entry.second;
        REQUIRE(testdata::MyTestMessage5::ID() == env.dataType());

        REQUIRE(1000 == env.sent().seconds());
        REQUIRE(1 == env.sent().microseconds());
        REQUIRE(5000 == env.received().seconds());
        REQUIRE(1 == env.received().microseconds());
        REQUIRE(10000 == env.sampleTimeStamp().seconds());
        REQUIRE(1 == env.sampleTimeStamp().microseconds());
        REQUIRE(player.hasMoreData());
    }

    UNLINK("rec7");
}


