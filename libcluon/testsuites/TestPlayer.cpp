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

#include "cluon/Player.hpp"

#include <fstream>

TEST_CASE("Create simple player for non existing file.") {
    constexpr bool AUTO_REWIND{false};
    constexpr bool THREADING{false};

    {
        std::fstream fileDoesNotExist("/pmt/this/file/does/not/exist", std::ios::in);
        REQUIRE(!fileDoesNotExist.good());
    }
    cluon::Player player("/pmt/this/file/does/not/exist", AUTO_REWIND, THREADING);

    REQUIRE(!player.hasMoreData());
}

