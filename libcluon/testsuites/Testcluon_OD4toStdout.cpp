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
#include "cluon/cluonTestDataStructures.hpp"
#include "cluon/TerminateHandler.hpp"

#include <chrono>
#include <string>
#include <sstream>
#include <streambuf>
#include <thread>

class RedirectCOUT {
   public:
    RedirectCOUT(std::streambuf *rdbuf)
     : m_rdbuf(std::cout.rdbuf(rdbuf))
    {}

    ~RedirectCOUT() {
        std::cout.rdbuf(m_rdbuf);
    }

   private:
    std::streambuf *m_rdbuf;
};

TEST_CASE("Test empty commandline parameters.") {
    int32_t argc       = 1;
    const char *argv[] = {static_cast<const char *>("cluon-OD4toStdout")};
    REQUIRE(1 == cluon_OD4toStdout(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test wrong --cid.") {
    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-OD4toStdout"), static_cast<const char *>("--cid=345")};
    REQUIRE(1 == cluon_OD4toStdout(argc, const_cast<char **>(argv)));
}


TEST_CASE("Test starting cluon-OD4toStdout in thread and send one message.") {
    // Reset TerminateHandler.
    cluon::TerminateHandler::instance().isTerminated.store(false);

    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    std::thread runOD4toStdout([]() {
        constexpr int32_t argc = 2;
        const char *argv[] = {static_cast<const char *>("cluon-OD4toStdout"), static_cast<const char *>("--cid=71")};
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
    std::this_thread::sleep_for(100ms);

    const std::string tmp = capturedCout.str();

    REQUIRE(107 == tmp.size());
    REQUIRE(0xd == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(0))));
    REQUIRE(0xa4 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(1))));
    REQUIRE(0x66 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(2))));
    REQUIRE(0x0 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(3))));
    REQUIRE(0x0 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(4))));
    REQUIRE(0x8 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(5))));
    REQUIRE(0xea == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(6))));
    REQUIRE(0xd4 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(7))));
    REQUIRE(0x3 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(8))));
    REQUIRE(0x12 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(9))));
    REQUIRE(0x3a == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(10))));
    REQUIRE(0x8 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(11))));
    REQUIRE(0x3 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(12))));
    REQUIRE(0x10 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(13))));
    REQUIRE(0x5 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(14))));
    REQUIRE(0x18 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(15))));
    REQUIRE(0x67 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(16))));
    REQUIRE(0x20 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(17))));
    REQUIRE(0xcd == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(18))));
    REQUIRE(0x1 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(19))));
    REQUIRE(0x28 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(20))));
    REQUIRE(0x93 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(21))));
    REQUIRE(0x4e == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(22))));
    REQUIRE(0x30 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(23))));
    REQUIRE(0xa5 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(24))));
    REQUIRE(0x9c == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(25))));
    REQUIRE(0x1 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(26))));
    REQUIRE(0x38 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(27))));
    REQUIRE(0xb1 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(28))));
    REQUIRE(0xa8 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(29))));
    REQUIRE(0x3 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(30))));
    REQUIRE(0x40 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(31))));
    REQUIRE(0xa1 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(32))));
    REQUIRE(0x89 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(33))));
    REQUIRE(0x9 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(34))));
    REQUIRE(0x4d == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(35))));
    REQUIRE(0xc3 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(36))));
    REQUIRE(0xd3 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(37))));
    REQUIRE(0xad == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(38))));
    REQUIRE(0xc0 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(39))));
    REQUIRE(0x51 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(40))));
    REQUIRE(0x1 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(41))));
    REQUIRE(0x4d == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(42))));
    REQUIRE(0x84 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(43))));
    REQUIRE(0xd == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(44))));
    REQUIRE(0x4f == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(45))));
    REQUIRE(0x37 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(46))));
    REQUIRE(0x49 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(47))));
    REQUIRE(0xc0 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(48))));
    REQUIRE(0x5a == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(49))));
    REQUIRE(0x12 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(50))));
    REQUIRE(0x48 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(51))));
    REQUIRE(0x65 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(52))));
    REQUIRE(0x6c == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(53))));
    REQUIRE(0x6c == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(54))));
    REQUIRE(0x6f == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(55))));
    REQUIRE(0x20 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(56))));
    REQUIRE(0x63 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(57))));
    REQUIRE(0x6c == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(58))));
    REQUIRE(0x75 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(59))));
    REQUIRE(0x6f == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(60))));
    REQUIRE(0x6e == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(61))));
    REQUIRE(0x20 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(62))));
    REQUIRE(0x57 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(63))));
    REQUIRE(0x6f == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(64))));
    REQUIRE(0x72 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(65))));
    REQUIRE(0x6c == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(66))));
    REQUIRE(0x64 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(67))));
    REQUIRE(0x21 == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(68))));
    REQUIRE(0x1a == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(69))));
    REQUIRE(0xa == static_cast<uint32_t>(static_cast<uint8_t>(tmp.at(70))));

    // Remaining bytes (71-106) cover timestamps.

    cluon::TerminateHandler::instance().isTerminated.store(true);

    runOD4toStdout.join();
}

