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

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
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
        std::map<int32_t, std::map<uint32_t, cluon::data::Envelope> > mapOfLastEnvelopes;

        cluon::OD4Session od4Session(static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])),
            [&](cluon::data::Envelope &&envelope) noexcept {
            std::lock_guard<std::mutex> lck(mapOfLastEnvelopesMutex);

            // Update mapping for tupel (dataType, senderStamp) --> Envelope.
            std::map<uint32_t, cluon::data::Envelope> entry = mapOfLastEnvelopes[envelope.dataType()];
            entry[envelope.senderStamp()] = envelope;
            mapOfLastEnvelopes[envelope.dataType()] = entry;

            clearScreen();

            const auto LAST_TIME_POINT{envelope.received().seconds() * 1000 * 1000 + envelope.received().microseconds()};

            uint8_t y = 1;
            const uint8_t x = 1;
            for (auto e : mapOfLastEnvelopes) {
                for (auto ee : e.second) {
                    auto env = ee.second;
                    std::stringstream sstr;

                    sstr << "Envelope: " << std::setfill(' ') << std::setw(5) << env.dataType() << std::setw(0) << "/" << env.senderStamp() << "; " << "sent: " << formatTimeStamp(env.sent()) << "; sample: " << formatTimeStamp(env.sampleTimeStamp());
                    if (scopeOfMetaMessages.count(env.dataType()) > 0) {
                        sstr << "; " << scopeOfMetaMessages[env.dataType()].messageName();
                    }
                    else {
                        sstr << "; unknown data type";
                    }
                    sstr << std::endl;

                    const auto AGE{LAST_TIME_POINT - (env.received().seconds() * 1000 * 1000 + env.received().microseconds())};

                    Color c = Color::DEFAULT;
                    if (AGE <= 2 * 1000 * 1000) { c = Color::GREEN; }
                    if (AGE > 2 * 1000 * 1000 && AGE <= 5 * 1000 * 1000) { c = Color::YELLOW; }
                    if (AGE > 5 * 1000 * 1000) { c = Color::RED; }

                    writeText(c, y++, x, sstr.str());
                }
            }
        });

        if (od4Session.isRunning()) {
            using namespace std::literals::chrono_literals; // NOLINT
            while (od4Session.isRunning()) {
                std::this_thread::sleep_for(1s);
            }

            retVal = 0;
        }
    }
    return retVal;
}

#endif

