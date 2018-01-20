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

TEST_CASE("Test ltrim.") {
    std::string test1l{"ABC"};
    std::string test2l{" ABC"};
    std::string test3l{"ABC "};
    std::string test4l{" ABC "};
    std::string test5l;
    std::string test6l{"A BC"};
    std::string test7l{" A BC"};
    std::string test8l{"A BC "};
    std::string test9l{" A BC "};

    REQUIRE(3 == cluon::ltrim(test1l).size());
    REQUIRE(3 == cluon::ltrim(test2l).size());
    REQUIRE(4 == cluon::ltrim(test3l).size());
    REQUIRE(4 == cluon::ltrim(test4l).size());
    REQUIRE(cluon::ltrim(test5l).empty());
    REQUIRE(4 == cluon::ltrim(test6l).size());
    REQUIRE(4 == cluon::ltrim(test7l).size());
    REQUIRE(5 == cluon::ltrim(test8l).size());
    REQUIRE(5 == cluon::ltrim(test9l).size());
}

TEST_CASE("Test rtrim.") {
    std::string test1r{"ABC"};
    std::string test2r{" ABC"};
    std::string test3r{"ABC "};
    std::string test4r{" ABC "};
    std::string test5r;
    std::string test6r{"A BC"};
    std::string test7r{" A BC"};
    std::string test8r{"A BC "};
    std::string test9r{" A BC "};

    REQUIRE(3 == cluon::rtrim(test1r).size());
    REQUIRE(4 == cluon::rtrim(test2r).size());
    REQUIRE(3 == cluon::rtrim(test3r).size());
    REQUIRE(4 == cluon::rtrim(test4r).size());
    REQUIRE(cluon::rtrim(test5r).empty());
    REQUIRE(4 == cluon::rtrim(test6r).size());
    REQUIRE(5 == cluon::rtrim(test7r).size());
    REQUIRE(4 == cluon::rtrim(test8r).size());
    REQUIRE(5 == cluon::rtrim(test9r).size());
}

TEST_CASE("Test trim.") {
    std::string test1t{"ABC"};
    std::string test2t{" ABC"};
    std::string test3t{"ABC "};
    std::string test4t{" ABC "};
    std::string test5t;
    std::string test6t{"A BC"};
    std::string test7t{" A BC"};
    std::string test8t{"A BC "};
    std::string test9t{" A BC "};

    REQUIRE(3 == cluon::trim(test1t).size());
    REQUIRE(3 == cluon::trim(test2t).size());
    REQUIRE(3 == cluon::trim(test3t).size());
    REQUIRE(3 == cluon::trim(test4t).size());
    REQUIRE(cluon::trim(test5t).empty());
    REQUIRE(4 == cluon::trim(test6t).size());
    REQUIRE(4 == cluon::trim(test7t).size());
    REQUIRE(4 == cluon::trim(test8t).size());
    REQUIRE(4 == cluon::trim(test9t).size());
}
