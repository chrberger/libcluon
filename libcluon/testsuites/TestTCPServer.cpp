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

#include "cluon/TCPServer.hpp"
#include "cluon/TCPConnection.hpp"

#include <ctime>
#include <atomic>
#include <chrono>
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

TEST_CASE("Creating TCPServer and receive data from one connection.") {
    auto before = std::chrono::system_clock::now();

    // Setup data structures to receive data from UDPReceiver.
    std::atomic<bool> hasDataReceived{false};
    std::string data;
    std::string sender;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::shared_ptr<cluon::TCPConnection> > connections;

    REQUIRE(data.empty());
    REQUIRE(sender.empty());
    REQUIRE(!hasDataReceived);
    REQUIRE(connections.empty());

    cluon::TCPServer srv3(
        1235,
        [&hasDataReceived, &data, &sender, &timestamp, &connections](std::string &&from, std::shared_ptr<cluon::TCPConnection> connection) noexcept {
            sender = std::move(from);
            std::cout << "Got connection from " << sender << std::endl;
            connection->setOnNewData([&hasDataReceived, &data, &timestamp](std::string &&d, std::chrono::system_clock::time_point &&ts){
                data = std::move(d);
                timestamp = std::move(ts);
                hasDataReceived.store(true);
            });
            connection->setOnConnectionLost([](){
                std::cout << "Connection lost." << std::endl;
            });
            connections.push_back(connection);
        });

    REQUIRE(srv3.isRunning());

    cluon::TCPConnection conn3("127.0.0.1", 1235);

    std::string TEST_DATA{"Hello World"};
    const auto TEST_DATA_SIZE{TEST_DATA.size()};
    auto retVal2 = conn3.send(std::move(TEST_DATA));
    REQUIRE(TEST_DATA_SIZE == retVal2.first);
    REQUIRE(0 == retVal2.second);

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
    std::vector<std::shared_ptr<cluon::TCPConnection> > connections;

    REQUIRE(0 == hasDataReceived);
    REQUIRE(connections.empty());

    cluon::TCPServer srv3(
        1236,
        [&hasDataReceived, &dataMutex, &data, &connections](std::string &&from, std::shared_ptr<cluon::TCPConnection> connection) noexcept {
            std::cout << "Got connection from " << from << std::endl;
            connection->setOnNewData([&hasDataReceived, &dataMutex, &data](std::string &&d, std::chrono::system_clock::time_point &&){
                std::lock_guard<std::mutex> lck(dataMutex);
                data.push_back(std::move(d));
                hasDataReceived++;
            });
            connection->setOnConnectionLost([](){
                std::cout << "Connection lost." << std::endl;
            });
            connections.push_back(connection);
        });

    REQUIRE(srv3.isRunning());

    constexpr uint8_t MAX_CONNECTIONS{10};
    for(uint8_t i{0}; i < MAX_CONNECTIONS; i++) {
        cluon::TCPConnection conn3("127.0.0.1", 1236);

        std::string TEST_DATA{"Hello World " + std::to_string(i)};
        const auto TEST_DATA_SIZE{TEST_DATA.size()};
        auto retVal2 = conn3.send(std::move(TEST_DATA));
        REQUIRE(TEST_DATA_SIZE == retVal2.first);
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

