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

#include "cluon/cluon.hpp"

#include <string>

TEST_CASE("Test empty commandline parsing.") {
    constexpr int32_t argc = 1;
    const char *argv[] =
    {
        static_cast<const char*>("myBinary")
    };
    auto retVal = cluon::getCommandlineArguments(argc, const_cast<char**>(argv));

    REQUIRE(1 == retVal.size());
    REQUIRE(retVal["myBinary"] == "");
}

TEST_CASE("Test non-empty commandline parsing.") {
    constexpr int32_t argc = 4;
    const char *argv[] =
    {
        static_cast<const char*>("myBinary"),
        static_cast<const char*>("--cid=100"),
        static_cast<const char*>("--freq=10"),
        static_cast<const char*>("--verbose")
    };
    auto retVal = cluon::getCommandlineArguments(argc, const_cast<char**>(argv));
    REQUIRE(4 == retVal.size());

    REQUIRE(retVal["myBinary"] == "");
    REQUIRE(retVal["cid"] == "100");
    REQUIRE(retVal["freq"] == "10");
    REQUIRE(retVal["verbose"] == "1");
}
