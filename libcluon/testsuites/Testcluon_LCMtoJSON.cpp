/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon-LCMtoJSON.hpp"
#include "cluon/TerminateHandler.hpp"
#include "cluon/UDPSender.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <chrono>
#include <fstream>
#include <memory>
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
    const char *argv[] = {static_cast<const char *>("cluon-LCMtoJSON")};
    REQUIRE(1 == cluon_LCMtoJSON(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test wrong connection.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-LCMtoJSON"), static_cast<const char *>("339.255.76.67:7667")};
    REQUIRE(1 == cluon_LCMtoJSON(argc, const_cast<char **>(argv)));
#endif
}

TEST_CASE("Test starting cluon-LCMtoJSON in thread.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runLCMtoJSON([]() {
        constexpr int32_t argc = 2;
        const char *argv[]     = {static_cast<const char *>("cluon-LCMtoJSON"), static_cast<const char *>("239.255.76.67:7667")};
        REQUIRE(0 == cluon_LCMtoJSON(argc, const_cast<char **>(argv)));
    });

    // Wait before stopping.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runLCMtoJSON.join();
#endif
}

TEST_CASE("Test starting cluon-LCMtoJSON in thread and send one message results in empty JSON.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runLCMtoJSON([]() {
        constexpr int32_t argc = 2;
        const char *argv[]     = {static_cast<const char *>("cluon-LCMtoJSON"), static_cast<const char *>("239.255.76.67:7667")};
        REQUIRE(0 == cluon_LCMtoJSON(argc, const_cast<char **>(argv)));
    });

    // Wait before sending.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    // The following raw byte sequence was created using LCM v1.3.1 for the following message specification.
    //    testdata::MyTestMessage5 msg;
    //    msg.attribute1(3)
    //        .attribute2(-3)
    //        .attribute3(103)
    //        .attribute4(-103)
    //        .attribute5(10003)
    //        .attribute6(-10003)
    //        .attribute7(54321)
    //        .attribute8(-74321)
    //        .attribute9(-5.4321f)
    //        .attribute10(-50.4321)
    //        .attribute11("Hello cluon World!");

    const unsigned char msg[]
        = {0x4c, 0x43, 0x30, 0x32, 0x00, 0x00, 0x00, 0x00, 0x45, 0x58, 0x41, 0x4d, 0x50, 0x4c, 0x45, 0x00, 0x7d, 0x89, 0x61, 0x9e, 0xbf, 0x59, 0xc7,
           0x60, 0x03, 0xfd, 0x00, 0x67, 0xff, 0x99, 0x00, 0x00, 0x27, 0x13, 0xff, 0xff, 0xd8, 0xed, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd4, 0x31,
           0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x2b, 0xcf, 0xc0, 0xad, 0xd3, 0xc3, 0xc0, 0x49, 0x37, 0x4f, 0x0d, 0x84, 0x4d, 0x01, 0x00, 0x00, 0x00,
           0x13, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x63, 0x6c, 0x75, 0x6f, 0x6e, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x00};

    cluon::UDPSender lcm("239.255.76.67", 7667);
    std::string s(reinterpret_cast<const char *>(msg), 89);
    lcm.send(std::move(s));

    // Wait before stopping.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    REQUIRE(capturedCout.str() == "{}\n");

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runLCMtoJSON.join();
#endif
}

TEST_CASE("Test starting cluon-LCMtoJSON in thread with corrupt ODVD and send one message results in empty JSON.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("ABC1.odvd");

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runLCMtoJSON([]() {
        const char *input = R"(
message EXAMPLE [id = 30005] {
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
            = {static_cast<const char *>("cluon-LCMtoJSON"), static_cast<const char *>("--odvd=ABC1.odvd"), static_cast<const char *>("239.255.76.67:7667")};
        REQUIRE(0 == cluon_LCMtoJSON(argc, const_cast<char **>(argv)));
    });

    // Wait before sending.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    // The following raw byte sequence was created using LCM v1.3.1 for the following message specification.
    //    testdata::MyTestMessage5 msg;
    //    msg.attribute1(3)
    //        .attribute2(-3)
    //        .attribute3(103)
    //        .attribute4(-103)
    //        .attribute5(10003)
    //        .attribute6(-10003)
    //        .attribute7(54321)
    //        .attribute8(-74321)
    //        .attribute9(-5.4321f)
    //        .attribute10(-50.4321)
    //        .attribute11("Hello cluon World!");

    const unsigned char msg[]
        = {0x4c, 0x43, 0x30, 0x32, 0x00, 0x00, 0x00, 0x00, 0x45, 0x58, 0x41, 0x4d, 0x50, 0x4c, 0x45, 0x00, 0x7d, 0x89, 0x61, 0x9e, 0xbf, 0x59, 0xc7,
           0x60, 0x03, 0xfd, 0x00, 0x67, 0xff, 0x99, 0x00, 0x00, 0x27, 0x13, 0xff, 0xff, 0xd8, 0xed, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd4, 0x31,
           0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x2b, 0xcf, 0xc0, 0xad, 0xd3, 0xc3, 0xc0, 0x49, 0x37, 0x4f, 0x0d, 0x84, 0x4d, 0x01, 0x00, 0x00, 0x00,
           0x13, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x63, 0x6c, 0x75, 0x6f, 0x6e, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x00};

    cluon::UDPSender lcm("239.255.76.67", 7667);
    std::string s(reinterpret_cast<const char *>(msg), 89);
    lcm.send(std::move(s));

    // Wait before stopping.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    REQUIRE(capturedCout.str() == "{}\n");

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runLCMtoJSON.join();

    UNLINK("ABC1.odvd");
#endif
}

TEST_CASE("Test starting cluon-LCMtoJSON in thread with valid ODVD and send one message results in valid JSON.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("ABC2.odvd");

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runLCMtoJSON([]() {
        // The message name must match the channel name in LCM.
        const char *input = R"(
message EXAMPLE [id = 30005] {
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
            = {static_cast<const char *>("cluon-LCMtoJSON"), static_cast<const char *>("--odvd=ABC2.odvd"), static_cast<const char *>("239.255.76.67:7667")};
        REQUIRE(0 == cluon_LCMtoJSON(argc, const_cast<char **>(argv)));
    });

    // Wait before sending.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    // The following raw byte sequence was created using LCM v1.3.1 for the following message specification.
    //    testdata::MyTestMessage5 msg;
    //    msg.attribute1(3)
    //        .attribute2(-3)
    //        .attribute3(103)
    //        .attribute4(-103)
    //        .attribute5(10003)
    //        .attribute6(-10003)
    //        .attribute7(54321)
    //        .attribute8(-74321)
    //        .attribute9(-5.4321f)
    //        .attribute10(-50.4321)
    //        .attribute11("Hello cluon World!");

    const unsigned char msg[]
        = {0x4c, 0x43, 0x30, 0x32, 0x00, 0x00, 0x00, 0x00, 0x45, 0x58, 0x41, 0x4d, 0x50, 0x4c, 0x45, 0x00, 0x7d, 0x89, 0x61, 0x9e, 0xbf, 0x59, 0xc7,
           0x60, 0x03, 0xfd, 0x00, 0x67, 0xff, 0x99, 0x00, 0x00, 0x27, 0x13, 0xff, 0xff, 0xd8, 0xed, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd4, 0x31,
           0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x2b, 0xcf, 0xc0, 0xad, 0xd3, 0xc3, 0xc0, 0x49, 0x37, 0x4f, 0x0d, 0x84, 0x4d, 0x01, 0x00, 0x00, 0x00,
           0x13, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x63, 0x6c, 0x75, 0x6f, 0x6e, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x00};

    cluon::UDPSender lcm("239.255.76.67", 7667);
    std::string s(reinterpret_cast<const char *>(msg), 89);
    lcm.send(std::move(s));

    // Wait before stopping.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    const char *expectedOutput = R"({"attribute1":3,
"attribute2":-3,
"attribute3":103,
"attribute4":-103,
"attribute5":10003,
"attribute6":-10003,
"attribute7":54321,
"attribute8":-54321,
"attribute9":-5.4321,
"attribute10":-50.4321,
"attribute11":"SGVsbG8gY2x1b24gV29ybGQh"}
)";

    REQUIRE(capturedCout.str() == std::string(expectedOutput));

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runLCMtoJSON.join();

    UNLINK("ABC2.odvd");
#endif
}
