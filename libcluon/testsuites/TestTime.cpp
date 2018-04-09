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

TEST_CASE("Test convert Time.") {
    cluon::data::TimeStamp before{cluon::time::now()};

    using namespace std::literals::chrono_literals; // NOLINT
    std::this_thread::sleep_for(1s);

    std::chrono::system_clock::time_point after = std::chrono::system_clock::now();

    cluon::data::TimeStamp after2 = cluon::time::convert(after);

    REQUIRE(cluon::time::toMicroseconds(after2) > cluon::time::toMicroseconds(before));
}
