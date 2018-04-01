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

#include "cluon/cluon.hpp"
#include "cluon/OD4Session.hpp"

#include <chrono>
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

void clearScreen();
void writeText(Color c, uint8_t y, uint8_t x, const std::string &text);
std::string formatTimeStamp(const cluon::data::TimeStamp &ts);

void clearScreen() {
    std::cout << "\033[2J" << std::endl;
}

void writeText(Color c, uint8_t y, uint8_t x, const std::string &text) {
    std::cout << "\033[" << +y << ";" << +x << "H" << "\033[0;" << +c << "m" << text << "\033[0m" << std::endl;
}

std::string formatTimeStamp(const cluon::data::TimeStamp &ts) {
    std::time_t temp = static_cast<std::time_t>(ts.seconds());
    std::tm* t = std::gmtime(&temp);
    std::stringstream sstr;
    sstr << std::put_time(t, "%H:%M:%S") << "." << std::setfill('0') << std::setw(6) << ts.microseconds() << std::setw(0);
    const std::string str{sstr.str()};
    return str;
}

int main(int argc, char **argv) {
    int retVal{1};
    const std::string PROGRAM{argv[0]}; // NOLINT
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (0 == commandlineArguments.count("cid")) {
        std::cerr << PROGRAM
                  << " displays any Envelopes received from an OpenDaVINCI v4 session to stdout." << std::endl;
        std::cerr << "Usage:    " << PROGRAM << " --cid=<OpenDaVINCI session>" << std::endl;
        std::cerr << "Examples: " << PROGRAM << " --cid=111" << std::endl;
    } else {
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

                    sstr << "Envelope: " << std::setfill(' ') << std::setw(5) << env.dataType() << std::setw(0) << "/" << env.senderStamp() << "; " << "sent: " << formatTimeStamp(env.sent()) << "; sample: " << formatTimeStamp(env.sampleTimeStamp()) << std::endl;

                    const auto AGE{LAST_TIME_POINT - (env.received().seconds() * 1000 * 1000 + env.received().microseconds())};

                    Color c = Color::DEFAULT;
                    if (AGE <= 2 * 1000 * 1000) { c = Color::GREEN; }
                    if (AGE > 2 * 1000 * 1000 && AGE <= 5 * 1000 * 1000) { c = Color::YELLOW; }
                    if (AGE > 5 * 1000 * 1000) { c = Color::RED; }

                    writeText(c, y++, x, sstr.str());
                }
            }
        });

        using namespace std::literals::chrono_literals; // NOLINT
        while (od4Session.isRunning()) {
            std::this_thread::sleep_for(1s);
        }

        retVal = 0;
    }
    return retVal;
}
