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

#include "cluon/SharedMemory.hpp"

TEST_CASE("Trying to open SharedMemory with empty name.") {
    cluon::SharedMemory sm1{""};
    REQUIRE(!sm1.valid());
    REQUIRE(nullptr == sm1.data());
    REQUIRE(sm1.name().empty());
}

TEST_CASE("Trying to open SharedMemory with name without leading /.") {
    cluon::SharedMemory sm1{"ABC"};
    REQUIRE(!sm1.valid());
    REQUIRE(nullptr == sm1.data());
    REQUIRE("/ABC" == sm1.name());
}

TEST_CASE("Trying to open SharedMemory with name without leading / and too long name > 255.") {
    const std::string NAME{"Vlrel3r6cZeWaRsWgvCWfAHtpPKX56fSgNYNM5bMjEcBnuiMOG3g4YJ4Y9KbPcNyes45xPI9jD5FjxEB1GR9WqaWmyqdH6po1O6is2aDecMe8GGlwqkVJtWH5YwlCYgoJ1EiQhqIUVfzp56IY00J6lXJS0uVJrpcMIZuiCsTGTQDG0vPC2EkdbMxe9BPV6a8BnMMumnGKYcqFxiCGrv1SVtLw40zLXTuelQQHiPCFANYlISyhRPt456PMNm7AQJUMHA5"};
    const std::string NAME_254 = "/" + NAME.substr(0, 253);
    cluon::SharedMemory sm1{NAME};
    REQUIRE(!sm1.valid());
    REQUIRE(nullptr == sm1.data());
    REQUIRE(NAME_254 == sm1.name());
}

TEST_CASE("Trying to create SharedMemory with correct name.") {
    cluon::SharedMemory sm1{"/DEF", 4};
    REQUIRE(sm1.valid());
    REQUIRE(nullptr != sm1.data());
    REQUIRE("/DEF" == sm1.name());
    sm1.lock();
    uint32_t *data = reinterpret_cast<uint32_t*>(sm1.data());
    *data = 12345;
    sm1.unlock();

    sm1.lock();
    uint32_t *data2 = reinterpret_cast<uint32_t*>(sm1.data());
    uint32_t tmp = *data2;
    sm1.unlock();
    REQUIRE(12345 == tmp);
}

