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

#include "cluon-replay.hpp"
#include "cluon/TerminateHandler.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <chrono>
#include <sstream>
#include <streambuf>
#include <string>
#include <thread>

// clang-format off
#ifdef WIN32
    #define UNLINK _unlink
#else
    #include <unistd.h>
    #define UNLINK unlink
#endif
// clang-format on

class RedirectCOUT {
   public:
    RedirectCOUT(std::streambuf *rdbuf)
        : m_rdbuf(std::cout.rdbuf(rdbuf)) {
        std::ios::sync_with_stdio(true);
    }

    ~RedirectCOUT() { std::cout.rdbuf(m_rdbuf); }

   private:
    std::streambuf *m_rdbuf;
};

TEST_CASE("Test empty commandline parameters.") {
    int32_t argc       = 1;
    const char *argv[] = {static_cast<const char *>("cluon-replay")};
    REQUIRE(1 == cluon_replay(argc, const_cast<char **>(argv), false /*do not monitor STDIN*/));
}

TEST_CASE("Test non-existing rec-file.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("abc.rec");

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-replay"), static_cast<const char *>("abc.rec")};
    REQUIRE(1 == cluon_replay(argc, const_cast<char **>(argv), false /*do not monitor STDIN*/));

    UNLINK("abc.rec");
#endif
}

TEST_CASE("Test playback rec-file to stdout.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("abc1.rec");

    constexpr int32_t MAX_ENTRIES{5};
    {
        std::fstream recordingFile("abc1.rec", std::ios::out | std::ios::binary | std::ios::trunc);
        REQUIRE(recordingFile.good());

        for (int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter + 1);

            cluon::ToProtoVisitor proto;
            msg.accept(proto);

            cluon::data::Envelope env;
            cluon::data::TimeStamp sent;
            sent.seconds(entryCounter / 2).microseconds(entryCounter);
            cluon::data::TimeStamp received;
            received.seconds(entryCounter / 2).microseconds(entryCounter);
            cluon::data::TimeStamp sampleTimeStamp;
            sampleTimeStamp.seconds(entryCounter / 2).microseconds(entryCounter);

            env.serializedData(proto.encodedData());
            env.dataType(testdata::MyTestMessage5::ID()).sent(sent).received(received).sampleTimeStamp(sampleTimeStamp);

            const std::string tmp{cluon::serializeEnvelope(std::move(env))};
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-replay"), static_cast<const char *>("abc1.rec")};
    REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv), false /*do not monitor STDIN*/));

    const std::string tmp = capturedCout.str();
    REQUIRE(!tmp.empty());

    UNLINK("abc1.rec");
#endif
}

TEST_CASE("Test playback rec-file to OD4Session.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("abc2.rec");

    constexpr int32_t MAX_ENTRIES{5};
    {
        std::fstream recordingFile("abc2.rec", std::ios::out | std::ios::binary | std::ios::trunc);
        REQUIRE(recordingFile.good());

        for (int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter + 1);

            cluon::ToProtoVisitor proto;
            msg.accept(proto);

            cluon::data::Envelope env;
            cluon::data::TimeStamp sent;
            sent.seconds(entryCounter / 2).microseconds(entryCounter);
            cluon::data::TimeStamp received;
            received.seconds(entryCounter / 2).microseconds(entryCounter);
            cluon::data::TimeStamp sampleTimeStamp;
            sampleTimeStamp.seconds(entryCounter / 2).microseconds(entryCounter);

            env.serializedData(proto.encodedData());
            env.dataType(testdata::MyTestMessage5::ID()).sent(sent).received(received).sampleTimeStamp(sampleTimeStamp);

            const std::string tmp{cluon::serializeEnvelope(std::move(env))};
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }

    uint32_t envelopeCounter{0};
    cluon::OD4Session od4(73, [&envelopeCounter](cluon::data::Envelope &&env) {
        if (env.dataType() == testdata::MyTestMessage5::ID()) {
            envelopeCounter++;
        }
    });

    using namespace std::literals::chrono_literals;
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 3;
    const char *argv[]     = {static_cast<const char *>("cluon-replay"), static_cast<const char *>("--cid=73"), static_cast<const char *>("abc2.rec")};
    REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv), false /*do not monitor STDIN*/));

    const std::string tmp = capturedCout.str();
    REQUIRE(tmp.empty());

    cluon::TerminateHandler::instance().isTerminated.store(true);

    REQUIRE(MAX_ENTRIES == envelopeCounter);
    UNLINK("abc2.rec");
#endif
}

TEST_CASE("Test playback rec-file to OD4Session with wrong cid results in playback to Stdout.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("abc3.rec");

    constexpr int32_t MAX_ENTRIES{5};
    {
        std::fstream recordingFile("abc3.rec", std::ios::out | std::ios::binary | std::ios::trunc);
        REQUIRE(recordingFile.good());

        for (int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter + 1);

            cluon::ToProtoVisitor proto;
            msg.accept(proto);

            cluon::data::Envelope env;
            cluon::data::TimeStamp sent;
            sent.seconds(entryCounter / 2).microseconds(entryCounter);
            cluon::data::TimeStamp received;
            received.seconds(entryCounter / 2).microseconds(entryCounter);
            cluon::data::TimeStamp sampleTimeStamp;
            sampleTimeStamp.seconds(entryCounter / 2).microseconds(entryCounter);

            env.serializedData(proto.encodedData());
            env.dataType(testdata::MyTestMessage5::ID()).sent(sent).received(received).sampleTimeStamp(sampleTimeStamp);

            const std::string tmp{cluon::serializeEnvelope(std::move(env))};
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 3;
    const char *argv[]     = {static_cast<const char *>("cluon-replay"), static_cast<const char *>("--cid=374"), static_cast<const char *>("abc3.rec")};
    REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv), false /*do not monitor STDIN*/));

    const std::string tmp = capturedCout.str();
    REQUIRE(!tmp.empty());

    cluon::TerminateHandler::instance().isTerminated.store(true);

    UNLINK("abc3.rec");
#endif
}

TEST_CASE("Test playback rec-file to OD4Session and to stdout.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("abc4.rec");

    constexpr int32_t MAX_ENTRIES{5};
    {
        std::fstream recordingFile("abc4.rec", std::ios::out | std::ios::binary | std::ios::trunc);
        REQUIRE(recordingFile.good());

        for (int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter + 1);

            cluon::ToProtoVisitor proto;
            msg.accept(proto);

            cluon::data::Envelope env;
            cluon::data::TimeStamp sent;
            sent.seconds(entryCounter / 2).microseconds(entryCounter);
            cluon::data::TimeStamp received;
            received.seconds(entryCounter / 2).microseconds(entryCounter);
            cluon::data::TimeStamp sampleTimeStamp;
            sampleTimeStamp.seconds(entryCounter / 2).microseconds(entryCounter);

            env.serializedData(proto.encodedData());
            env.dataType(testdata::MyTestMessage5::ID()).sent(sent).received(received).sampleTimeStamp(sampleTimeStamp);

            const std::string tmp{cluon::serializeEnvelope(std::move(env))};
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }

    uint32_t envelopeCounter{0};
    cluon::OD4Session od4(74, [&envelopeCounter](cluon::data::Envelope &&env) {
        if (env.dataType() == testdata::MyTestMessage5::ID()) {
            envelopeCounter++;
        }
    });

    using namespace std::literals::chrono_literals;
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 4;
    const char *argv[]     = {static_cast<const char *>("cluon-replay"),
                          static_cast<const char *>("--cid=74"),
                          static_cast<const char *>("--stdout"),
                          static_cast<const char *>("abc4.rec")};
    REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv), false /*do not monitor STDIN*/));

    const std::string tmp = capturedCout.str();
    REQUIRE(!tmp.empty());

    cluon::TerminateHandler::instance().isTerminated.store(true);

    REQUIRE(MAX_ENTRIES == envelopeCounter);
    UNLINK("abc4.rec");
#endif
}

TEST_CASE("Test playback rec-file to OD4Session with external player control.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("abc5.rec");

    constexpr int32_t MAX_ENTRIES{5};
    {
        std::fstream recordingFile("abc5.rec", std::ios::out | std::ios::binary | std::ios::trunc);
        REQUIRE(recordingFile.good());

        for (int32_t entryCounter{0}; entryCounter < MAX_ENTRIES; entryCounter++) {
            testdata::MyTestMessage5 msg;
            msg.attribute6(entryCounter + 1);

            cluon::ToProtoVisitor proto;
            msg.accept(proto);

            cluon::data::Envelope env;
            cluon::data::TimeStamp sent;
            sent.seconds(entryCounter * 10).microseconds(entryCounter);
            cluon::data::TimeStamp received;
            received.seconds(entryCounter * 10).microseconds(entryCounter);
            cluon::data::TimeStamp sampleTimeStamp;
            sampleTimeStamp.seconds(entryCounter * 10).microseconds(entryCounter);

            env.serializedData(proto.encodedData());
            env.dataType(testdata::MyTestMessage5::ID()).sent(sent).received(received).sampleTimeStamp(sampleTimeStamp);

            const std::string tmp{cluon::serializeEnvelope(std::move(env))};
            recordingFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
            recordingFile.flush();
        }
        recordingFile.close();
    }

    bool seeked{false};
    uint32_t playerStatusCounter{0};
    uint32_t envelopeCounter{0};
    cluon::OD4Session od4Sender(75);
    cluon::OD4Session od4(75, [&seeked, &playerStatusCounter, &envelopeCounter, &od4Sender](cluon::data::Envelope &&env) {
        if (!seeked && envelopeCounter == 2) {
            cluon::data::PlayerCommand pc;
            pc.command(2 /* pause */);
            od4Sender.send(pc);
        }
        if (!seeked && playerStatusCounter == 2) {
            cluon::data::PlayerCommand pc;
            pc.command(1 /* play */);
            od4Sender.send(pc);
        }
        if (!seeked && envelopeCounter == 3) {
            seeked = true;
            cluon::data::PlayerCommand pc;
            pc.command(3 /* seekTo */);
            pc.seekTo(0 /* go to beginning */);
            od4Sender.send(pc);
        }
        if (env.dataType() == testdata::MyTestMessage5::ID()) {
            envelopeCounter++;
        }
        if (env.dataType() == cluon::data::PlayerStatus::ID()) {
            playerStatusCounter++;
        }
    });

    using namespace std::literals::chrono_literals;
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runcluon_replay([]() {
        constexpr int32_t argc = 3;
        const char *argv[]     = {static_cast<const char *>("cluon-replay"), static_cast<const char *>("--cid=75"), static_cast<const char *>("abc5.rec")};
        REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv), false /*do not monitor STDIN*/));
    });

    // Join parallel thread.
    runcluon_replay.join();

    // Only playback to OD4Session.
    const std::string tmp = capturedCout.str();
    REQUIRE(tmp.empty());

    cluon::TerminateHandler::instance().isTerminated.store(true);

    // We have seeked to the beginning of the file after 3 entries and thus, we must replay more than 5 entries.
    REQUIRE(MAX_ENTRIES < envelopeCounter);
    UNLINK("abc5.rec");
#endif
}
