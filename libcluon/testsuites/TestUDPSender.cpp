/*
 * Copyright (C) 2017  Christian Berger
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

#include "cluon/UDPSender.hpp"

#include <cerrno>
#include <string>
#include <utility>

// Defining a test fixture to be reused among the test cases.
class TestFixture_UDPSender {
   public:
    TestFixture_UDPSender()
        : m_us("127.0.0.1", 5678) {}

   protected:
    cluon::UDPSender m_us;
};

TEST_CASE_METHOD(TestFixture_UDPSender, "Creating UDPSender.") {}

TEST_CASE_METHOD(TestFixture_UDPSender, "Send test data.") {
    std::string TEST_DATA{"Hello World"};
    const auto TEST_DATA_SIZE = TEST_DATA.size();
    auto retVal2              = m_us.send(std::move(TEST_DATA));
    REQUIRE(TEST_DATA_SIZE == retVal2.first);
    REQUIRE(0 == retVal2.second);
}

TEST_CASE_METHOD(TestFixture_UDPSender, "Send empty data.") {
    std::string TEST_DATA;
    auto retVal2 = m_us.send(std::move(TEST_DATA));
    REQUIRE(0 == retVal2.first);
    REQUIRE(0 == retVal2.second);
}

TEST_CASE("Trying to send data with faulty sender.") {
    cluon::UDPSender us3{"127.0.0.256", 5677};
    std::string TEST_DATA{"Hello World"};
    auto retVal3 = us3.send(std::move(TEST_DATA));
    REQUIRE(-1 == retVal3.first);
#ifdef WIN32
    constexpr int32_t EXPECTED_VALUE = 9;
#else
    constexpr int32_t EXPECTED_VALUE = EBADF;
#endif
    REQUIRE(EXPECTED_VALUE == retVal3.second);
}

TEST_CASE_METHOD(TestFixture_UDPSender, "Trying to send too big data.") {
    std::string TEST_DATA(0xFFFF - 1, 'A');
    auto retVal4 = m_us.send(std::move(TEST_DATA));
    REQUIRE(-1 == retVal4.first);
    REQUIRE(E2BIG == retVal4.second);
}

TEST_CASE("Trying to send data with empty sendToAddress.") {
    cluon::UDPSender us5{"", 1};
    std::string TEST_DATA{"Hello World"};
    auto retVal5 = us5.send(std::move(TEST_DATA));
    REQUIRE(-1 == retVal5.first);
#ifdef WIN32
    constexpr int32_t EXPECTED_VALUE = 9;
#else
    constexpr int32_t EXPECTED_VALUE = EBADF;
#endif
    REQUIRE(EXPECTED_VALUE == retVal5.second);
}

TEST_CASE("Trying to send data with empty sendToPort.") {
    cluon::UDPSender us6{"127.0.0.1", 0};
    std::string TEST_DATA{"Hello World"};
    auto retVal6 = us6.send(std::move(TEST_DATA));
    REQUIRE(-1 == retVal6.first);
#ifdef WIN32
    constexpr int32_t EXPECTED_VALUE = 9;
#else
    constexpr int32_t EXPECTED_VALUE = EBADF;
#endif
    REQUIRE(EXPECTED_VALUE == retVal6.second);
}

TEST_CASE("Trying to send data with incomplete sendToAddress.") {
    cluon::UDPSender us6{"127.0.0", 1};
    std::string TEST_DATA{"Hello World"};
    auto retVal6 = us6.send(std::move(TEST_DATA));
    REQUIRE(-1 == retVal6.first);
#ifdef WIN32
    constexpr int32_t EXPECTED_VALUE = 9;
#else
    constexpr int32_t EXPECTED_VALUE = EBADF;
#endif
    REQUIRE(EXPECTED_VALUE == retVal6.second);
}
