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

#include "cluon/ProtoConstants.hpp"

TEST_CASE("Test constants.") {
    REQUIRE(0 == static_cast<uint8_t>(cluon::ProtoConstants::VARINT));
    REQUIRE(1 == static_cast<uint8_t>(cluon::ProtoConstants::EIGHT_BYTES));
    REQUIRE(2 == static_cast<uint8_t>(cluon::ProtoConstants::LENGTH_DELIMITED));
    REQUIRE(5 == static_cast<uint8_t>(cluon::ProtoConstants::FOUR_BYTES));
}
