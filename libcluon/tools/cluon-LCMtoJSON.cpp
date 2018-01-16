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
#include "cluon/LCMToGenericMessage.hpp"
#include "cluon/ToJSONVisitor.hpp"

#include "argh/argh.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <thread>

int main(int argc, char **argv) {
    int retVal{1};
    const std::string PROGRAM{argv[0]}; // NOLINT
    argh::parser commandline{argc, argv};
    std::string LCM{commandline.pos_args().back()};
    if (std::string::npos != LCM.find(PROGRAM)) {
        std::cerr << PROGRAM
                  << " dumps LCM payload received from an LCM session to stdout in JSON format using an optionally supplied ODVD message specification file." << std::endl;
        std::cerr << "Usage:    " << PROGRAM << " [--odvd=<ODVD message specification file>] a.b.c.d:port" << std::endl;
        std::cerr << "Examples: " << PROGRAM << " 239.255.76.67:7667" << std::endl;
        std::cerr << "          " << PROGRAM << " --odvd=MyMessages.odvd 239.255.76.67:7667" << std::endl;
    } else {
        std::string tmp{LCM};
        std::replace(tmp.begin(), tmp.end(), ':', ' ');
        std::istringstream sstr{tmp};
        std::vector<std::string> tokens{std::istream_iterator<std::string>(sstr), std::istream_iterator<std::string>()};

        const std::string ADDRESS{tokens.at(0)};
        const uint16_t PORT{static_cast<uint16_t>(std::stoi(tokens.at(1)))};

        std::string odvdFile;
        commandline({"--odvd"}) >> odvdFile;

        cluon::LCMToGenericMessage lcm2GM;
        if (!odvdFile.empty()) {
            std::fstream fin{odvdFile, std::ios::in};
            if (fin.good()) {
                const std::string s{static_cast<std::stringstream const&>(std::stringstream() << fin.rdbuf()).str()}; // NOLINT
                std::clog << "Parsed " << lcm2GM.setMessageSpecification(s) << " message(s)." << std::endl;
            }
        }

        cluon::UDPReceiver receiver(
            ADDRESS, PORT,
            [&l2GM = lcm2GM](std::string && data, std::string &&, std::chrono::system_clock::time_point &&) noexcept {
                cluon::GenericMessage gm = l2GM.getGenericMessage(data);
                cluon::ToJSONVisitor j;
                gm.accept(j);
                std::cout << j.json() << std::endl;
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
