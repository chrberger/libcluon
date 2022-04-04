/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon/TCPConnection.hpp"
#include "cluon/TCPServer.hpp"

#include <atomic>
#include <cerrno>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

TEST_CASE("Creating TCPServer and stop immediately.") {
    cluon::TCPServer srv1{1234, nullptr};
    REQUIRE(srv1.isRunning());
}

TEST_CASE("Trying to receive with wrong sendToPort.") {
    cluon::TCPServer failingSrv2{0, nullptr};
    REQUIRE(!failingSrv2.isRunning());
}

TEST_CASE("Trying to connect to wrong sendToPort.") {
    cluon::TCPConnection conn1("256.0.0.1", 0);
    REQUIRE(!conn1.isRunning());

    std::string TEST_DATA{"Hello World"};
    auto retVal2 = conn1.send(std::move(TEST_DATA));
    REQUIRE(-1 == retVal2.first);
#ifdef WIN32
    constexpr int32_t EXPECTED_VALUE = 9;
#else
    constexpr int32_t EXPECTED_VALUE = EBADF;
#endif
    REQUIRE(EXPECTED_VALUE == retVal2.second);
}

TEST_CASE("Creating TCPServer and receive data from one connection.") {
    auto before = std::chrono::system_clock::now();

    // Setup data structures to receive data from UDPReceiver.
    std::atomic<bool> hasDataReceived{false};
    std::string data;
    std::string sender;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::shared_ptr<cluon::TCPConnection>> connections;

    REQUIRE(data.empty());
    REQUIRE(sender.empty());
    REQUIRE(!hasDataReceived);
    REQUIRE(connections.empty());

    cluon::TCPServer srv3(
        1235, [&hasDataReceived, &data, &sender, &timestamp, &connections ](std::string && from, std::shared_ptr<cluon::TCPConnection> connection) noexcept {
            sender = std::move(from);
            std::cout << "Got connection from " << sender << std::endl;
            connection->setOnNewData([&hasDataReceived, &data, &timestamp](std::string &&d, std::chrono::system_clock::time_point &&ts) {
                data      = std::move(d);
                timestamp = std::move(ts);
                hasDataReceived.store(true);
            });
            connection->setOnConnectionLost([]() { std::cout << "Connection lost." << std::endl; });
            connections.push_back(connection);
        });
    REQUIRE(srv3.isRunning());

    cluon::TCPConnection conn3("127.0.0.1", 1235);
    REQUIRE(conn3.isRunning());

    std::string TEST_DATA{"Hello World"};
    const auto TEST_DATA_SIZE{TEST_DATA.size()};
    auto retVal2 = conn3.send(std::move(TEST_DATA));
    REQUIRE(TEST_DATA_SIZE == static_cast<unsigned int>(retVal2.first));
    REQUIRE(0 == retVal2.second);

    {
        // Try sending empty data.
        auto retVal3 = conn3.send("");
        REQUIRE(0 == retVal3.first);
        REQUIRE(0 == retVal3.second);
    }

    {
        // Try sending too large data.
        std::string TEST_DATA2(0xFFFF + 1, 'A');
        auto retVal4 = conn3.send(std::move(TEST_DATA2));
        REQUIRE(-1 == retVal4.first);
        REQUIRE(E2BIG == retVal4.second);
    }

    // Yield the UDP receiver so that the embedded thread has time to process the data.
    // Let the operating system spawn the thread.
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

TEST_CASE("Creating TCPServer and receive data from multiple connections.") {
    // Setup data structures to receive data from UDPReceiver.
    std::atomic<uint8_t> hasDataReceived{0};
    std::mutex dataMutex;
    std::vector<std::string> data;
    std::vector<std::shared_ptr<cluon::TCPConnection>> connections;

    REQUIRE(0 == hasDataReceived);
    REQUIRE(connections.empty());

    cluon::TCPServer srv4(1236,
                          [&hasDataReceived, &dataMutex, &data, &connections ](std::string && from, std::shared_ptr<cluon::TCPConnection> connection) noexcept {
                              std::cout << "Got connection from " << from << std::endl;
                              connection->setOnNewData([&hasDataReceived, &dataMutex, &data](std::string &&d, std::chrono::system_clock::time_point &&) {
                                  std::lock_guard<std::mutex> lck(dataMutex);
                                  data.push_back(std::move(d));
                                  hasDataReceived++;
                              });
                              connection->setOnConnectionLost([]() { std::cout << "Connection lost." << std::endl; });
                              connections.push_back(connection);
                          });

    REQUIRE(srv4.isRunning());

    constexpr uint8_t MAX_CONNECTIONS{10};
    for (uint8_t i{0}; i < MAX_CONNECTIONS; i++) {
        cluon::TCPConnection conn4("127.0.0.1", 1236);
        REQUIRE(conn4.isRunning());

        std::string TEST_DATA{"Hello World " + std::to_string(i)};
        const auto TEST_DATA_SIZE{TEST_DATA.size()};
        auto retVal2 = conn4.send(std::move(TEST_DATA));
        REQUIRE(TEST_DATA_SIZE == static_cast<unsigned int>(retVal2.first));
        REQUIRE(0 == retVal2.second);

        using namespace std::literals::chrono_literals; // NOLINT
        std::this_thread::sleep_for(1ms);
    }

    // Yield the UDP receiver so that the embedded thread has time to process the data.
    // Let the operating system spawn the thread.
    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (hasDataReceived != MAX_CONNECTIONS);

    REQUIRE(MAX_CONNECTIONS == hasDataReceived);
    REQUIRE(MAX_CONNECTIONS == data.size());
}
