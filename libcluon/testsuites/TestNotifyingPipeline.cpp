/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
    } catch (...) { REQUIRE(false); } // LCOV_EXCL_LINE
}
