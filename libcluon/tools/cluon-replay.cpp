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
#include <string>
#include <thread>

int main(int argc, char **argv) {
    int32_t retCode{0};
    const std::string PROGRAM{argv[0]}; // NOLINT
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (0 == commandlineArguments.count("cid")) {
        std::cerr << PROGRAM << " replays a .rec file into an OpenDaVINCI session." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " --cid=<OpenDaVINCI session> recording.rec" << std::endl;
        std::cerr << "Example: " << PROGRAM << " --cid=111 file.rec" << std::endl;
        retCode = 1;
    }
    else {
        // Interface to a running OpenDaVINCI session (ignoring any incoming Envelopes).
        cluon::OD4Session od4(static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])), [](auto){});
        if (od4.isRunning()) {
            std::string recFile;
            for (auto e : commandlineArguments) {
                if (recFile.empty() && e.second.empty() && e.first != PROGRAM) {
                    recFile = e.first;
                    break;
                }
            }

            std::fstream fin(recFile, std::ios::in|std::ios::binary);
            if (fin.good()) {
                int32_t oldTimeStampInMicroseconds{0};
                while (fin.good()) {
                    cluon::data::Envelope envelope{cluon::extractEnvelope(fin)};
                    if (envelope.dataType() > 0) {
                        const auto SENT{envelope.sent()};
                        const int32_t CURRENT_TIMESTAMP_IN_MICROSECONDS{(SENT.seconds()*1000*1000 + SENT.microseconds())};
                        const int32_t DELAY = CURRENT_TIMESTAMP_IN_MICROSECONDS - oldTimeStampInMicroseconds;
                        std::this_thread::sleep_for(std::chrono::duration<int32_t, std::micro>(DELAY > 0 ? DELAY : 0));
                        od4.send(std::move(envelope));
                        oldTimeStampInMicroseconds = CURRENT_TIMESTAMP_IN_MICROSECONDS;
                    }
                }
            }
            else {
                std::cerr << "[" << PROGRAM << "] '" << recFile << "' could not be opened." << std::endl;
            }
        }
    }
    return retCode;
}
