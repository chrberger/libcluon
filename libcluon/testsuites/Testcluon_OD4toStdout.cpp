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

#include "cluon-OD4toStdout.hpp"
#include "cluon/Player.hpp"
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
    const char *argv[] = {static_cast<const char *>("cluon-OD4toStdout")};
    REQUIRE(1 == cluon_OD4toStdout(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test wrong --cid.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-OD4toStdout"), static_cast<const char *>("--cid=345")};
    REQUIRE(1 == cluon_OD4toStdout(argc, const_cast<char **>(argv)));
#endif
}

TEST_CASE("Test starting cluon-OD4toStdout in thread and send one message.") {
// Test only on x86_64 platforms.
#if defined(__amd64__) && defined(__linux__)
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    UNLINK("GHI.rec");

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runOD4toStdout([]() {
        constexpr int32_t argc = 2;
        const char *argv[]     = {static_cast<const char *>("cluon-OD4toStdout"), static_cast<const char *>("--cid=71")};
        REQUIRE(0 == cluon_OD4toStdout(argc, const_cast<char **>(argv)));
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

    cluon::OD4Session od4(71);

    using namespace std::literals::chrono_literals;
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    od4.send(msg);

    // Wait before stopping.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(500ms);
    capturedCout.flush();

    const std::string tmp = capturedCout.str();

    // Write received data into file and replay with Player.
    {
        std::fstream fout("GHI.rec", std::ios::out | std::ios::binary | std::ios::trunc);
        fout << tmp;
        fout.flush();
        fout.close();

        constexpr bool AUTO_REWIND{false};
        constexpr bool THREADING{false};
        cluon::Player player("GHI.rec", AUTO_REWIND, THREADING);

        REQUIRE(player.hasMoreData());
        REQUIRE(1 == player.totalNumberOfEnvelopesInRecFile());

        auto entry = player.getNextEnvelopeToBeReplayed();
        REQUIRE(entry.first);

        cluon::data::Envelope env = entry.second;
        REQUIRE(testdata::MyTestMessage5::ID() == env.dataType());

        testdata::MyTestMessage5 tmp2 = cluon::extractMessage<testdata::MyTestMessage5>(std::move(env));
        REQUIRE(3 == tmp2.attribute1());
        REQUIRE(-3 == tmp2.attribute2());
        REQUIRE(103 == tmp2.attribute3());
        REQUIRE(-103 == tmp2.attribute4());
        REQUIRE(10003 == tmp2.attribute5());
        REQUIRE(-10003 == tmp2.attribute6());
        REQUIRE(54321 == tmp2.attribute7());
        REQUIRE(-74321 == tmp2.attribute8());
        REQUIRE(-5.4321f == Approx(tmp2.attribute9()));
        REQUIRE(-50.4321 == Approx(tmp2.attribute10()));
        REQUIRE("Hello cluon World!" == tmp2.attribute11());
    }

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runOD4toStdout.join();
    UNLINK("GHI.rec");
#endif
}
