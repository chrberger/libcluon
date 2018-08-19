/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon/cluon.hpp"

#include <string>

TEST_CASE("Test empty commandline parsing.") {
    constexpr int32_t argc = 1;
    const char *argv[]     = {static_cast<const char *>("myBinary")};
    auto retVal            = cluon::getCommandlineArguments(argc, const_cast<char **>(argv));

    REQUIRE(1 == retVal.size());
    REQUIRE(retVal["myBinary"] == "");
}

TEST_CASE("Test non-empty commandline parsing.") {
    constexpr int32_t argc = 4;
    const char *argv[]     = {static_cast<const char *>("myBinary"),
                          static_cast<const char *>("--cid=100"),
                          static_cast<const char *>("--freq=10"),
                          static_cast<const char *>("--verbose")};
    auto retVal            = cluon::getCommandlineArguments(argc, const_cast<char **>(argv));
    REQUIRE(4 == retVal.size());

    REQUIRE(retVal["myBinary"] == "");
    REQUIRE(retVal["cid"] == "100");
    REQUIRE(retVal["freq"] == "10");
    REQUIRE(retVal["verbose"] == "1");
}
