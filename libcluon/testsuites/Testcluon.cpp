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
    std::string argv0("myBinary");
    int32_t argc = 1;
    char **argv;
    argv    = new char *[1];
    argv[0] = const_cast<char *>(argv0.c_str());

    auto retVal = cluon::getCommandlineArguments(argc, argv);
    REQUIRE(1 == retVal.size());
    REQUIRE(retVal["myBinary"] == "");
}

TEST_CASE("Test non-empty commandline parsing.") {
    std::string argv0("myBinary");
    std::string argv1("--cid=100");
    std::string argv2("--freq=10");
    std::string argv3("--verbose");
    int32_t argc = 4;
    char **argv;
    argv    = new char *[4];
    argv[0] = const_cast<char *>(argv0.c_str());
    argv[1] = const_cast<char *>(argv1.c_str());
    argv[2] = const_cast<char *>(argv2.c_str());
    argv[3] = const_cast<char *>(argv3.c_str());

    auto retVal = cluon::getCommandlineArguments(argc, argv);
    REQUIRE(4 == retVal.size());

    REQUIRE(retVal["myBinary"] == "");
    REQUIRE(retVal["cid"] == "100");
    REQUIRE(retVal["freq"] == "10");
    REQUIRE(retVal["verbose"] == "1");
}
