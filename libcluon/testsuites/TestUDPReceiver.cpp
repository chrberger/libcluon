/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon/UDPReceiver.hpp"
#include "cluon/UDPSender.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

TEST_CASE("Creating UDPReceiver and stop immediately.") {
    cluon::UDPReceiver ur1{"127.0.0.1", 1234, nullptr};
    REQUIRE(ur1.isRunning());
}

TEST_CASE("Trying to send data with empty sendToAddress.") {
    cluon::UDPReceiver failingUR1{"", 1235, nullptr};
    REQUIRE(!failingUR1.isRunning());
}

TEST_CASE("Trying to receive with wrong sendToPort.") {
    cluon::UDPReceiver failingUR2{"127.0.0.1", 0, nullptr};
    REQUIRE(!failingUR2.isRunning());
}

TEST_CASE("Trying to send data with incomplete sendToAddress.") {
    cluon::UDPReceiver failingUR3{"127.0.0", 1236, nullptr};
    REQUIRE(!failingUR3.isRunning());
}

TEST_CASE("Trying to send data with wrong sendToAddress.") {
    cluon::UDPReceiver failingUR3_2{"127.0.0.256", 1233, nullptr};
    REQUIRE(!failingUR3_2.isRunning());
}

TEST_CASE("Creating UDPReceiver and receive data.") {
    auto before = std::chrono::system_clock::now();

    // Setup data structures to receive data from UDPReceiver.
    std::atomic<bool> hasDataReceived{false};
    std::string data;
    std::string sender;
    std::chrono::system_clock::time_point timestamp;

    REQUIRE(data.empty());
    REQUIRE(sender.empty());
    REQUIRE(!hasDataReceived);

    cluon::UDPReceiver ur3(
        "127.0.0.1",
        1237,
        [&hasDataReceived, &data, &sender, &timestamp ](std::string && d, std::string && s, std::chrono::system_clock::time_point && ts) noexcept {
            data      = std::move(d);
            sender    = std::move(s);
            timestamp = ts;
            hasDataReceived.store(true);
            std::time_t tp = std::chrono::system_clock::to_time_t(timestamp);
            std::cout << "Received '" << data << "' at '" << tp << ""
                      << "'" << std::endl;
        });

    REQUIRE(ur3.isRunning());

    cluon::UDPSender us2{"127.0.0.1", 1237};
    std::string TEST_DATA{"Hello World"};
    const auto TEST_DATA_SIZE{TEST_DATA.size()};
    auto retVal2 = us2.send(std::move(TEST_DATA));
    REQUIRE(TEST_DATA_SIZE == retVal2.first);
    REQUIRE(0 == retVal2.second);

    // Yield the UDP receiver so that the embedded thread has time to process the data.
    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!hasDataReceived.load());

    REQUIRE(hasDataReceived);
    REQUIRE("Hello World" == data);
    REQUIRE(sender.substr(0, sender.find(':')) == "127.0.0.1");

    auto after = std::chrono::system_clock::now();

    // Test if the timestamping works correctly (only on 64bit systems).
    if (8 == sizeof(void *)) {
        REQUIRE(before < timestamp);
        REQUIRE(timestamp < after);
    }
}

TEST_CASE("Testing multicast with 226.x.y.z address.") {
    // Setup data structures to receive data from UDPReceiver.
    std::atomic<bool> hasDataReceived{false};
    std::string data;

    REQUIRE(data.empty());
    REQUIRE(!hasDataReceived);

    cluon::UDPReceiver ur4(
        "226.0.0.226", 1238, [&hasDataReceived, &data ](std::string && d, std::string &&, std::chrono::system_clock::time_point &&) noexcept {
            data = std::move(d);
            hasDataReceived.store(true);
        });
    REQUIRE(ur4.isRunning());

    // Setup UDPSender.
    cluon::UDPSender us4{"226.0.0.226", 1238};
    std::string TEST_DATA{"Hello Multicast Group"};
    const auto TEST_DATA_SIZE{TEST_DATA.size()};
    auto retVal4 = us4.send(std::move(TEST_DATA));
    REQUIRE(TEST_DATA_SIZE == retVal4.first);
    REQUIRE(0 == retVal4.second);

    // Yield the UDP receiver so that the embedded thread has time to process the data.
    do { std::this_thread::yield(); } while (!hasDataReceived.load());

    REQUIRE(hasDataReceived);
    REQUIRE("Hello Multicast Group" == data);
}

TEST_CASE("Testing multicast with faulty 224.0.0.1 address.") {
    // Setup data structures to receive data from UDPReceiver.
    std::atomic<bool> hasDataReceived{false};

    REQUIRE(!hasDataReceived);

    // Setup UDPReceiver.
    cluon::UDPReceiver ur5{"224.0.0.1", 1239, nullptr};
    REQUIRE(!ur5.isRunning());
    REQUIRE(!hasDataReceived);
}
