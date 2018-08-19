/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon-livefeed.hpp"
#include "cluon/TerminateHandler.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <chrono>
#include <fstream>
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
    const char *argv[] = {static_cast<const char *>("cluon-livefeed")};
    REQUIRE(1 == cluon_livefeed(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test wrong --cid.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-livefeed"), static_cast<const char *>("--cid=345")};
    REQUIRE(1 == cluon_livefeed(argc, const_cast<char **>(argv)));
#endif
}

TEST_CASE("Test starting cluon-livefeed in thread.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runlivefeed([]() {
        constexpr int32_t argc = 2;
        const char *argv[]     = {static_cast<const char *>("cluon-livefeed"), static_cast<const char *>("--cid=67")};
        REQUIRE(0 == cluon_livefeed(argc, const_cast<char **>(argv)));
    });

    // Wait before stopping.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runlivefeed.join();
#endif
}

TEST_CASE("Test starting cluon-livefeed in thread and send one message results in default display.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runlivefeed([]() {
        constexpr int32_t argc = 2;
        const char *argv[]     = {static_cast<const char *>("cluon-livefeed"), static_cast<const char *>("--cid=68")};
        REQUIRE(0 == cluon_livefeed(argc, const_cast<char **>(argv)));
    });

    // Wait before sending.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    testdata::MyTestMessage5 msg;
    msg.attribute1(3)
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

    cluon::OD4Session od4(68);

    using namespace std::literals::chrono_literals;
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    od4.send(msg);

    // Wait before stopping.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    const std::string tmp = capturedCout.str();
    REQUIRE(std::string::npos != tmp.find("Envelope:"));
    REQUIRE(std::string::npos == tmp.find("MyTestMessage5"));

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runlivefeed.join();
#endif
}

TEST_CASE("Test starting cluon-livefeed in thread with corrupt ODVD and send one message results in default display.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("ABC1.odvd");

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runlivefeed([]() {
        const char *input = R"(
message testdata.MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
)";
        std::string messageSpecification(input);

        std::fstream odvd("ABC1.odvd", std::ios::out);
        odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
        odvd.close();

        constexpr int32_t argc = 3;
        const char *argv[]
            = {static_cast<const char *>("cluon-livefeed"), static_cast<const char *>("--odvd=ABC1.odvd"), static_cast<const char *>("--cid=69")};
        REQUIRE(0 == cluon_livefeed(argc, const_cast<char **>(argv)));
    });

    // Wait before sending.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    testdata::MyTestMessage5 msg;
    msg.attribute1(3)
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

    cluon::OD4Session od4(69);

    using namespace std::literals::chrono_literals;
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    od4.send(msg);

    // Wait before stopping.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    const std::string tmp = capturedCout.str();
    REQUIRE(std::string::npos != tmp.find("Envelope:"));
    REQUIRE(std::string::npos == tmp.find("MyTestMessage5"));

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runlivefeed.join();

    UNLINK("ABC1.odvd");
#endif
}

TEST_CASE("Test starting cluon-livefeed in thread with valid ODVD and send one message results in message name resolving.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("ABC2.odvd");

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runlivefeed([]() {
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

        std::fstream odvd("ABC2.odvd", std::ios::out);
        odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
        odvd.close();

        constexpr int32_t argc = 3;
        const char *argv[]
            = {static_cast<const char *>("cluon-livefeed"), static_cast<const char *>("--odvd=ABC2.odvd"), static_cast<const char *>("--cid=70")};
        REQUIRE(0 == cluon_livefeed(argc, const_cast<char **>(argv)));
    });

    // Wait before sending.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    testdata::MyTestMessage5 msg;
    msg.attribute1(3)
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

    cluon::OD4Session od4(70);

    using namespace std::literals::chrono_literals;
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    od4.send(msg);

    // Wait before stopping.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    const std::string tmp = capturedCout.str();
    REQUIRE(std::string::npos != tmp.find("Envelope:"));
    REQUIRE(std::string::npos != tmp.find("MyTestMessage5"));

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runlivefeed.join();

    UNLINK("ABC2.odvd");
#endif
}
