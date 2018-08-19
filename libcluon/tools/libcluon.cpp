/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

std::string encodeEnvelopeFromJSONWithSampleTimeStamp(const std::string &s, int32_t messageIdentifier, uint32_t senderStamp) {
    std::lock_guard<std::mutex> lck(envConverterMutex);
    return envConverter.getProtoEncodedEnvelopeFromJSON(s, messageIdentifier, senderStamp);
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
    function("encodeEnvelopeFromJSONWithSampleTimeStamp", &encodeEnvelopeFromJSONWithSampleTimeStamp);
}
#endif
