/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon-replay.hpp"
#include "cluon/TerminateHandler.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <atomic>
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
    REQUIRE(1 == cluon_replay(argc, const_cast<char **>(argv)));
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
    REQUIRE(1 == cluon_replay(argc, const_cast<char **>(argv)));

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
    REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv)));

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
    REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv)));

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
    REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv)));

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
    REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv)));

    const std::string tmp = capturedCout.str();
    REQUIRE(!tmp.empty());

    cluon::TerminateHandler::instance().isTerminated.store(true);

    REQUIRE(MAX_ENTRIES == envelopeCounter);
    UNLINK("abc4.rec");
#endif
}

TEST_CASE("Test playback rec-file to OD4Session and stdout with external player control.") {
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
    bool paused{false};
    bool unpaused{false};
    uint32_t playerStatusCounter{0};
    uint32_t envelopeCounter{0};
    cluon::OD4Session od4Sender(75);
    cluon::OD4Session od4(75, [&seeked, &paused, &unpaused, &playerStatusCounter, &envelopeCounter, &od4Sender](cluon::data::Envelope &&env) {
        if (env.dataType() == testdata::MyTestMessage5::ID()) {
            envelopeCounter++;
        }
        if (env.dataType() == cluon::data::PlayerStatus::ID()) {
            playerStatusCounter++;
        }
        if (!paused && envelopeCounter == 2) {
            paused = true;
            cluon::data::PlayerCommand pc;
            pc.command(2 /* pause */);
            od4Sender.send(pc);
        }
        if (!unpaused && playerStatusCounter == 4) {
            unpaused = true;
            cluon::data::PlayerCommand pc;
            pc.command(1 /* play */);
            od4Sender.send(pc);
        }
        if (!seeked && envelopeCounter == 4) {
            seeked = true;
            cluon::data::PlayerCommand pc;
            pc.command(3 /* seekTo */);
            pc.seekTo(0 /* go to beginning */);
            od4Sender.send(pc);
        }
    });

    using namespace std::literals::chrono_literals;
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runcluon_replay([]() {
        constexpr int32_t argc = 4;
        const char *argv[]     = {static_cast<const char *>("cluon-replay"),
                              static_cast<const char *>("--cid=75"),
                              static_cast<const char *>("--stdout"),
                              static_cast<const char *>("abc5.rec")};
        REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv)));
    });

    // Join parallel thread.
    runcluon_replay.join();

    // Playback to OD4Session and stdout.
    const std::string tmp = capturedCout.str();
    REQUIRE(!tmp.empty());

    cluon::TerminateHandler::instance().isTerminated.store(true);

    // We have seeked to the beginning of the file after 3 entries and thus, we must replay more than 5 entries.
    REQUIRE(MAX_ENTRIES < envelopeCounter);
    UNLINK("abc5.rec");
#endif
}

TEST_CASE("Test playback rec-file to OD4Session with external player control for stepping.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("abc6.rec");

    constexpr int32_t MAX_ENTRIES{5};
    {
        std::fstream recordingFile("abc6.rec", std::ios::out | std::ios::binary | std::ios::trunc);
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

    bool playerPaused{false};
    uint32_t playerStatusCounter{0};
    std::atomic<uint32_t> envelopeCounter{0};
    cluon::OD4Session od4Sender(76);
    cluon::OD4Session od4(76, [&playerPaused, &playerStatusCounter, &envelopeCounter, &od4Sender](cluon::data::Envelope &&env) {
        if (env.dataType() == testdata::MyTestMessage5::ID()) {
            envelopeCounter++;
        }
        if (env.dataType() == cluon::data::PlayerStatus::ID()) {
            playerStatusCounter++;
        }
        if (!playerPaused && (envelopeCounter == 2)) {
            playerPaused = true;
            cluon::data::PlayerCommand pc;
            pc.command(2 /* pause */);
            od4Sender.send(pc);
        }
    });

    using namespace std::literals::chrono_literals;
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    std::thread runcluon_replay([]() {
        constexpr int32_t argc = 3;
        const char *argv[]     = {static_cast<const char *>("cluon-replay"),
                              static_cast<const char *>("--cid=76"),
                              static_cast<const char *>("abc6.rec")};
        REQUIRE(0 == cluon_replay(argc, const_cast<char **>(argv)));
    });

    do { std::this_thread::sleep_for(1ms); } while (envelopeCounter < 2);

    REQUIRE(2 == envelopeCounter);
    std::this_thread::sleep_for(500ms);
    REQUIRE(2 == envelopeCounter);

    {
        cluon::data::PlayerCommand pc;
        pc.command(4 /* step */);
        od4Sender.send(pc);
    }

    do { std::this_thread::sleep_for(1ms); } while (envelopeCounter < 3);

    REQUIRE(3 == envelopeCounter);
    std::this_thread::sleep_for(500ms);
    REQUIRE(3 == envelopeCounter);

    {
        cluon::data::PlayerCommand pc;
        pc.command(1 /* play */);
        od4Sender.send(pc);
    }

    // Join parallel thread.
    runcluon_replay.join();

    cluon::TerminateHandler::instance().isTerminated.store(true);

    // We have seeked to the beginning of the file after 3 entries and thus, we must replay more than 5 entries.
    REQUIRE(MAX_ENTRIES == envelopeCounter);
    UNLINK("abc6.rec");
#endif
}
