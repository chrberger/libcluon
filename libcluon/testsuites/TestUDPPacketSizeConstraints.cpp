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

#include "cluon/UDPPacketSizeConstraints.hpp"

TEST_CASE("Test constants.") {
    REQUIRE(20 == static_cast<uint16_t>(cluon::UDPPacketSizeConstraints::SIZE_IPv4_HEADER));
    REQUIRE(8 == static_cast<uint16_t>(cluon::UDPPacketSizeConstraints::SIZE_UDP_HEADER));
    REQUIRE(0xFFFF == static_cast<uint16_t>(cluon::UDPPacketSizeConstraints::MAX_SIZE_UDP_PACKET));
}
