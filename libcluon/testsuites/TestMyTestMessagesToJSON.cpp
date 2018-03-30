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

#include "cluon/FromJSONVisitor.hpp"
#include "cluon/ToJSONVisitor.hpp"
#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"
#include "cluon/cluonTestDataStructures.hpp"

#include <sstream>
#include <string>

TEST_CASE("Testing TimeStamp-1.") {
    cluon::data::TimeStamp ts1;
    REQUIRE(ts1.seconds() == 0);
    REQUIRE(ts1.microseconds() == 0);

    ts1.seconds(123).microseconds(456);

    cluon::ToJSONVisitor jsonEncoder;
    ts1.accept(jsonEncoder);

    const char *JSON = R"({"seconds":123,
"microseconds":456})";

    REQUIRE(std::string(JSON) == jsonEncoder.json());

    std::stringstream sstr{jsonEncoder.json()};
    cluon::FromJSONVisitor jsonDecoder;
    jsonDecoder.decodeFrom(sstr);

    cluon::data::TimeStamp ts2;
    REQUIRE(ts2.seconds() == 0);
    REQUIRE(ts2.microseconds() == 0);

    ts2.accept(jsonDecoder);

    REQUIRE(ts2.seconds() == ts1.seconds());
    REQUIRE(ts2.microseconds() == ts1.microseconds());
}

