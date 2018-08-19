/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon/Time.hpp"
#include "cluon/cluonDataStructures.hpp"
#include <chrono>
#include <thread>

TEST_CASE("Test Time.") {
    cluon::data::TimeStamp before{cluon::time::now()};

    using namespace std::literals::chrono_literals; // NOLINT
    std::this_thread::sleep_for(1s);

    cluon::data::TimeStamp after{cluon::time::now()};

    REQUIRE(cluon::time::toMicroseconds(after) > cluon::time::toMicroseconds(before));
}

TEST_CASE("Test delta.") {
    cluon::data::TimeStamp before;
    before.seconds(0).microseconds(100);

    cluon::data::TimeStamp after;
    after.seconds(0).microseconds(300);

    REQUIRE(200 == cluon::time::deltaInMicroseconds(after, before));
}

TEST_CASE("Test from microseconds.") {
    int64_t timePoint{static_cast<int64_t>(2 * 1000 * 1000) + 13};
    cluon::data::TimeStamp result = cluon::time::fromMicroseconds(timePoint);

    REQUIRE(2 == result.seconds());
    REQUIRE(13 == result.microseconds());
}

TEST_CASE("Test convert Time.") {
    cluon::data::TimeStamp before{cluon::time::now()};

    using namespace std::literals::chrono_literals; // NOLINT
    std::this_thread::sleep_for(1s);

    std::chrono::system_clock::time_point after = std::chrono::system_clock::now();

    cluon::data::TimeStamp after2 = cluon::time::convert(after);

    REQUIRE(cluon::time::toMicroseconds(after2) > cluon::time::toMicroseconds(before));
}
