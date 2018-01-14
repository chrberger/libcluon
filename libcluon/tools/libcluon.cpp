/*
 * Copyright (C) 2017  Christian Berger
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

// clang-format off
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/bind.h>
    using namespace emscripten;
#endif
// clang-format off

#include "cluon/EnvelopeToJSON.hpp"

#include <string>

static cluon::EnvelopeToJSON env2JSON;

int setMessageSpecification(const std::string &s);
std::string decodeEnvelopeToJSON(const std::string &s);

int setMessageSpecification(const std::string &s) {
    return env2JSON.setMessageSpecification(s);
}

std::string decodeEnvelopeToJSON(const std::string &s) {
    return env2JSON.getJSONFromProtoEncodedEnvelope(s);
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    return 0;
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(libcluon) {
    function("setMessageSpecification", &setMessageSpecification);
    function("decodeEnvelopeToJSON", &decodeEnvelopeToJSON);
}
#endif
