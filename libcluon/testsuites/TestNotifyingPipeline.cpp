/*
 * Copyright (C) 2017-2018  Christian Berger
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

#include "cluon/NotifyingPipeline.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

TEST_CASE("Creating a NotifyingPipeline and stop immediately.") {
    cluon::NotifyingPipeline<std::string> pipeline(nullptr);
    REQUIRE(pipeline.isRunning());
}

TEST_CASE("Creating a NotifyingPipeline, add one entry, and get notified.") {
    std::atomic<bool> hasDataReceived{false};
    std::string data;

    try {
        cluon::NotifyingPipeline<std::string> pipeline([&hasDataReceived, &data](std::string &&entry) {
            hasDataReceived.store(true);
            data = entry;
        });
        REQUIRE(pipeline.isRunning());
        REQUIRE(!hasDataReceived);
        REQUIRE(data.empty());

        std::string dataToSend("Hello World");
        pipeline.add(std::move(dataToSend));

        REQUIRE(!hasDataReceived);
        pipeline.notifyAll();

        // Yield the UDP receiver so that the embedded thread has time to process the data.
        using namespace std::literals::chrono_literals; // NOLINT
        do { std::this_thread::sleep_for(1ms); } while (!hasDataReceived.load());

        REQUIRE("Hello World" == data);
    } catch (...) { REQUIRE(false); }
}
