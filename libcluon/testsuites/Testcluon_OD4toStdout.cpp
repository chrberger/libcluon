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

#include "cluon-OD4toStdout.hpp"
#include "cluon/cluonTestDataStructures.hpp"
#include "cluon/TerminateHandler.hpp"

#include <chrono>
#include <string>
#include <sstream>
#include <streambuf>
#include <thread>

class RedirectCOUT {
   public:
    RedirectCOUT(std::streambuf *rdbuf)
     : m_rdbuf(std::cout.rdbuf(rdbuf))
    {}

    ~RedirectCOUT() {
        std::cout.rdbuf(m_rdbuf);
    }

   private:
    std::streambuf *m_rdbuf;
};

TEST_CASE("Test empty commandline parameters.") {
    int32_t argc       = 1;
    const char *argv[] = {static_cast<const char *>("cluon-OD4toStdout")};
    REQUIRE(1 == cluon_OD4toStdout(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test wrong --cid.") {
    std::stringstream capturedCout;
    RedirectCOUT redirect(capturedCout.rdbuf());

    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-OD4toStdout"), static_cast<const char *>("--cid=345")};
    REQUIRE(1 == cluon_OD4toStdout(argc, const_cast<char **>(argv)));
}


