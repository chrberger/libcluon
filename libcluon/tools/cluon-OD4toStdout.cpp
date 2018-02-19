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

#include "cluon/cluon.hpp"
#include "cluon/OD4Session.hpp"

#include <iostream>
#include <string>
#include <thread>

int main(int argc, char **argv) {
    int retVal{1};
    const std::string PROGRAM{argv[0]}; // NOLINT
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (0 == commandlineArguments.count("cid")) {
        std::cerr << PROGRAM
                  << " dumps Containers received from an OpenDaVINCI v4 session to stdout." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " --cid=<OpenDaVINCI session>" << std::endl;
        std::cerr << "Example: " << PROGRAM << " --cid=111" << std::endl;
    }
    else {
        // Interface to a running OpenDaVINCI session (ignoring any incoming Envelopes).
        cluon::OD4Session od4Session(static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])),
            [](cluon::data::Envelope &&envelope) noexcept {
                std::cout << cluon::OD4Session::serializeAsOD4Container(std::move(envelope));
                std::cout.flush();
            });

        using namespace std::literals::chrono_literals; // NOLINT
        while (od4Session.isRunning()) {
            std::this_thread::sleep_for(1s);
        }
        retVal = 0;
    }
    return retVal;
}
