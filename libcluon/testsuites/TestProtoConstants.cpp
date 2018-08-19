/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon/ProtoConstants.hpp"

TEST_CASE("Test constants.") {
    REQUIRE(0 == static_cast<uint8_t>(cluon::ProtoConstants::VARINT));
    REQUIRE(1 == static_cast<uint8_t>(cluon::ProtoConstants::EIGHT_BYTES));
    REQUIRE(2 == static_cast<uint8_t>(cluon::ProtoConstants::LENGTH_DELIMITED));
    REQUIRE(5 == static_cast<uint8_t>(cluon::ProtoConstants::FOUR_BYTES));
}
