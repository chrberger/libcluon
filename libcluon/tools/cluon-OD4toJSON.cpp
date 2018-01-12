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

#include "cluon/UDPReceiver.hpp"
#include "cluon/EnvelopeToJSON.hpp"

#include "argh/argh.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

int main(int argc, char **argv) {
    int retVal{1};
    const std::string PROGRAM{argv[0]}; // NOLINT
    argh::parser commandline{argc, argv};
    std::string CID{commandline.pos_args().back()};
    if (std::string::npos != CID.find(PROGRAM)) {
        std::cerr << PROGRAM
                  << " dumps Containers received from an OpenDaVINCI v4 session to stdout in JSON format using an optionally supplied ODVD message specification file." << std::endl;
        std::cerr << "Usage:    " << PROGRAM << " [--odvd=<ODVD message specification file>] CID" << std::endl;
        std::cerr << "Examples: " << PROGRAM << " 111" << std::endl;
        std::cerr << "          " << PROGRAM << " --odvd=MyMessages.odvd 111" << std::endl;
    } else {
        const std::string ADDRESS{"225.0.0." + CID};
        constexpr uint16_t PORT{12175};

        std::string odvdFile;
        commandline({"--odvd"}) >> odvdFile;

        cluon::EnvelopeToJSON envelopeToJSON;
        if (!odvdFile.empty()) {
            std::fstream fin{odvdFile, std::ios::in};
            if (fin.good()) {
                const std::string s{static_cast<std::stringstream const&>(std::stringstream() << fin.rdbuf()).str()}; // NOLINT
                std::clog << "Parsed " << envelopeToJSON.setMessageSpecification(s) << " message(s)." << std::endl;
            }
        }

        cluon::UDPReceiver receiver(
            ADDRESS, PORT,
            [&e2J = envelopeToJSON](std::string && data, std::string &&, std::chrono::system_clock::time_point &&) noexcept {
                std::cout << e2J.getJSONFromProtoEncodedEnvelope(data) << std::endl;
                std::cout.flush();
            });

        using namespace std::literals::chrono_literals; // NOLINT
        while (receiver.isRunning()) {
            std::this_thread::sleep_for(1s);
        }
        retVal = 0;
    }
    return retVal;
}
