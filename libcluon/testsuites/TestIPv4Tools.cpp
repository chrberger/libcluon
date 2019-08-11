/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon/IPv4Tools.hpp"

TEST_CASE("Test hostname resolution localhost to 127.0.0.1.") {
    std::string resolvedHostname = cluon::getIPv4FromHostname("localhost");
    REQUIRE(resolvedHostname == "127.0.0.1");
}

TEST_CASE("Test hostname resolution localhos to empty string.") {
    std::string resolvedHostname = cluon::getIPv4FromHostname("localhos");
    REQUIRE(resolvedHostname == "");
}

TEST_CASE("Test hostname resolution 127.0.0.1 to 127.0.0.1.") {
    std::string resolvedHostname = cluon::getIPv4FromHostname("127.0.0.1");
    REQUIRE(resolvedHostname == "127.0.0.1");
}

TEST_CASE("Test hostname resolution 127.0.1 to 127.0.0.1.") {
    std::string resolvedHostname = cluon::getIPv4FromHostname("127.0.1");
    REQUIRE(resolvedHostname == "127.0.0.1");
}
