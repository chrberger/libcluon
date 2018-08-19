/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon/UDPPacketSizeConstraints.hpp"

TEST_CASE("Test constants.") {
    REQUIRE(20 == static_cast<uint16_t>(cluon::UDPPacketSizeConstraints::SIZE_IPv4_HEADER));
    REQUIRE(8 == static_cast<uint16_t>(cluon::UDPPacketSizeConstraints::SIZE_UDP_HEADER));
    REQUIRE(0xFFFF == static_cast<uint16_t>(cluon::UDPPacketSizeConstraints::MAX_SIZE_UDP_PACKET));
}
