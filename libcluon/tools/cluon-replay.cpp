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
#include "cluon/Envelope.hpp"
#include "cluon/OD4Session.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

int main(int argc, char **argv) {
    int32_t retCode{0};
    const std::string PROGRAM{argv[0]}; // NOLINT
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (1 == argc) {
        std::cerr << PROGRAM << " replays a .rec file into an OpenDaVINCI session or to stdout." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " [--cid=<OpenDaVINCI session>] recording.rec" << std::endl;
        std::cerr << "Example: " << PROGRAM << " --cid=111 file.rec" << std::endl;
        retCode = 1;
    }
    else {
        std::string recFile;
        for (auto e : commandlineArguments) {
            if (recFile.empty() && e.second.empty() && e.first != PROGRAM) {
                recFile = e.first;
                break;
            }
        }

        std::fstream fin(recFile, std::ios::in|std::ios::binary);
        if (fin.good()) {
            std::unique_ptr<cluon::OD4Session> od4;
            if (0 != commandlineArguments.count("cid")) {
                // Interface to a running OpenDaVINCI session (ignoring any incoming Envelopes).
                od4 = std::make_unique<cluon::OD4Session>(static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])), [](auto){});
            }

            int32_t oldTimeStampInMicroseconds{0};
            while (fin.good()) {
                auto retVal{cluon::extractEnvelope(fin)};
                if (retVal.first) {
                    if (retVal.second.dataType() > 0) {
                        const auto SENT{retVal.second.sent()};
                        const int32_t CURRENT_TIMESTAMP_IN_MICROSECONDS{(SENT.seconds()*1000*1000 + SENT.microseconds())};
                        const int32_t DELAY = CURRENT_TIMESTAMP_IN_MICROSECONDS - oldTimeStampInMicroseconds;
                        std::this_thread::sleep_for(std::chrono::duration<int32_t, std::micro>(DELAY > 0 ? DELAY : 0));

                        if (od4) {
                            if (od4->isRunning()) {
                                od4->send(std::move(retVal.second));
                            }
                        }
                        else {
                            std::cout << cluon::serializeEnvelope(std::move(retVal.second));
                            std::cout.flush();
                        }
                        oldTimeStampInMicroseconds = CURRENT_TIMESTAMP_IN_MICROSECONDS;
                    }
                }
            }
        }
        else {
            std::cerr << "[" << PROGRAM << "] '" << recFile << "' could not be opened." << std::endl;
        }
    }
    return retCode;
}
