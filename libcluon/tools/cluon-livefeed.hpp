/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_LIVEFEED_HPP
#define CLUON_LIVEFEED_HPP

#include "cluon/cluon.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/MessageParser.hpp"
#include "cluon/OD4Session.hpp"
#include "cluon/TerminateHandler.hpp"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

enum Color {
    RED     = 31,
    GREEN   = 32,
    YELLOW  = 33,
    DEFAULT = 39,
};

inline void clearScreen() {
    std::cout << "\033[2J" << std::endl;
}

inline void writeText(Color c, uint8_t y, uint8_t x, const std::string &text) {
    std::cout << "\033[" << +y << ";" << +x << "H" << "\033[0;" << +c << "m" << text << "\033[0m" << std::endl;
}

inline std::string formatTimeStamp(const cluon::data::TimeStamp &ts) {
    std::time_t temp = static_cast<std::time_t>(ts.seconds());
    std::tm* t = std::gmtime(&temp);
    std::stringstream sstr;
    sstr << std::put_time(t, "%H:%M:%S") << "." << std::setfill('0') << std::setw(6) << ts.microseconds() << std::setw(0);
    const std::string str{sstr.str()};
    return str;
}

inline int32_t cluon_livefeed(int32_t argc, char **argv) {
    int retVal{1};
    const std::string PROGRAM{argv[0]}; // NOLINT
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (0 == commandlineArguments.count("cid")) {
        std::cerr << PROGRAM
                  << " displays any Envelopes received from an OpenDaVINCI v4 session to stdout with optional data type resolving using a .odvd message specification." << std::endl;
        std::cerr << "Usage:    " << PROGRAM << " [--odvd=<ODVD message specification file>] --cid=<OpenDaVINCI session>" << std::endl;
        std::cerr << "Examples: " << PROGRAM << " --cid=111" << std::endl;
        std::cerr << "          " << PROGRAM << " --odvd=MyMessages.odvd --cid=111" << std::endl;
    } else {
        std::map<int32_t, cluon::MetaMessage> scopeOfMetaMessages{};

        // Try parsing a supplied .odvd file to resolve numerical data types to human readable message names.
        {
            std::string odvdFile{commandlineArguments["odvd"]};
            if (!odvdFile.empty()) {
                std::fstream fin{odvdFile, std::ios::in};
                if (fin.good()) {
                    const std::string s{static_cast<std::stringstream const&>(std::stringstream() << fin.rdbuf()).str()}; // NOLINT

                    cluon::MessageParser mp;
                    auto parsingResult = mp.parse(s);
                    if (!parsingResult.first.empty()) {
                        for (const auto &mm : parsingResult.first) { scopeOfMetaMessages[mm.messageIdentifier()] = mm; }
                        std::clog << "Parsed " << parsingResult.first.size() << " message(s)." << std::endl;
                    }
                }
            }
        }

        std::mutex mapOfLastEnvelopesMutex;
        std::unordered_map<int32_t, std::unordered_map<uint32_t, cluon::data::Envelope, cluon::UseUInt32ValueAsHashKey>, cluon::UseUInt32ValueAsHashKey> mapOfLastEnvelopes;
        std::unordered_map<int32_t, std::unordered_map<uint32_t, float>, cluon::UseUInt32ValueAsHashKey> mapOfUpdateRates;

        cluon::OD4Session od4Session(static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])),
            [&mapOfLastEnvelopesMutex, &mapOfLastEnvelopes, &mapOfUpdateRates](cluon::data::Envelope &&envelope) noexcept {
            std::lock_guard<std::mutex> lck(mapOfLastEnvelopesMutex);

            int64_t lastTimeStamp{0};
            int64_t currentTimeStamp{0};
            {
                // Update mapping for tupel (dataType, senderStamp) --> Envelope.
                auto entry = mapOfLastEnvelopes[envelope.dataType()];
                if (0 != entry.count(envelope.senderStamp())) {
                    lastTimeStamp = cluon::time::toMicroseconds(entry[envelope.senderStamp()].sampleTimeStamp()); // LCOV_EXCL_LINE
                }
                currentTimeStamp = cluon::time::toMicroseconds(envelope.sampleTimeStamp()); // LCOV_EXCL_LINE
                if (currentTimeStamp != lastTimeStamp) {
                    entry[envelope.senderStamp()] = envelope; // LCOV_EXCL_LINE
                    mapOfLastEnvelopes[envelope.dataType()] = entry; // LCOV_EXCL_LINE
                }
            }
            if (currentTimeStamp != lastTimeStamp) {
                // Update mapping for tupel (dataType, senderStamp) --> deltaToLastEnvelope.
                auto entry = mapOfUpdateRates[envelope.dataType()];

                float average{0};
                if (0 != entry.count(envelope.senderStamp())) {
                    average = entry[envelope.senderStamp()]; // LCOV_EXCL_LINE
                    float freq = (static_cast<float>(currentTimeStamp - lastTimeStamp))/(1000.0f*1000.0f); // LCOV_EXCL_LINE
                    average = (1.0f/freq)*0.1f + 0.9f*average; // LCOV_EXCL_LINE
                }
                entry[envelope.senderStamp()] = average;
                mapOfUpdateRates[envelope.dataType()] = entry;
            }
        });

        if (od4Session.isRunning()) {
            od4Session.timeTrigger(5, [&mapOfLastEnvelopesMutex, &mapOfLastEnvelopes, &mapOfUpdateRates, &scopeOfMetaMessages, &od4Session](){
                std::lock_guard<std::mutex> lck(mapOfLastEnvelopesMutex);

                if (!mapOfLastEnvelopes.empty()) {
                    clearScreen();

                    uint8_t y = 1;
                    const uint8_t x = 1;
                    for (auto e : mapOfLastEnvelopes) {
                        for (auto ee : e.second) {
                            auto env = ee.second;
                            std::stringstream sstr;

                            float freq{0};
                            if ( (0 < mapOfUpdateRates.count(ee.second.dataType())) && (0 < mapOfUpdateRates[ee.second.dataType()].count(ee.second.senderStamp())) ) {
                                freq = mapOfUpdateRates[ee.second.dataType()][ee.second.senderStamp()];
                            }

                            sstr << "Envelope: " << std::setfill(' ') << std::setw(5) << env.dataType() << std::setw(0) << "/" << env.senderStamp() << "; " << (static_cast<float>(static_cast<uint32_t>(freq*100.0f))/100.f) << " Hz; " << "sent: " << formatTimeStamp(env.sent()) << "; sample: " << formatTimeStamp(env.sampleTimeStamp());
                            if (scopeOfMetaMessages.count(env.dataType()) > 0) {
                                sstr << "; " << scopeOfMetaMessages[env.dataType()].messageName();
                            }
                            else {
                                sstr << "; unknown data type";
                            }
                            sstr << std::endl;

                            const auto AGE{cluon::time::deltaInMicroseconds(cluon::time::now(), env.received())};

                            Color c = Color::DEFAULT;
                            if (AGE <= 2 * 1000 * 1000) { c = Color::GREEN; }
                            if (AGE > 2 * 1000 * 1000 && AGE <= 5 * 1000 * 1000) { c = Color::YELLOW; }
                            if (AGE > 5 * 1000 * 1000) { c = Color::RED; }

                            writeText(c, y++, x, sstr.str());
                        }
                    }
                }
                return od4Session.isRunning();
            });

            retVal = 0;
        }
    }
    return retVal;
}

#endif

