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

// clang-format off
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/bind.h>
    using namespace emscripten;
#endif
// clang-format off

#include "cluon/EnvelopeConverter.hpp"

#include <mutex>
#include <sstream>
#include <string>

static std::mutex envConverterMutex;
static cluon::EnvelopeConverter envConverter;

int setMessageSpecification(const std::string &s);
std::string decodeEnvelopeToJSON(const std::string &s);
std::string encodeEnvelopeFromJSONWithoutTimeStamps(const std::string &s, int32_t messageIdentifier, uint32_t senderStamp);

int setMessageSpecification(const std::string &s) {
    std::lock_guard<std::mutex> lck(envConverterMutex);
    return envConverter.setMessageSpecification(s);
}

std::string decodeEnvelopeToJSON(const std::string &s) {
    std::lock_guard<std::mutex> lck(envConverterMutex);
    return envConverter.getJSONFromProtoEncodedEnvelope(s);
}

std::string encodeEnvelopeFromJSONWithoutTimeStamps(const std::string &s, int32_t messageIdentifier, uint32_t senderStamp) {
    std::lock_guard<std::mutex> lck(envConverterMutex);
    return envConverter.getProtoEncodedEnvelopeFromJSONWithoutTimeStamps(s, messageIdentifier, senderStamp);
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
    function("encodeEnvelopeFromJSONWithoutTimeStamps", &encodeEnvelopeFromJSONWithoutTimeStamps);
}
#endif
