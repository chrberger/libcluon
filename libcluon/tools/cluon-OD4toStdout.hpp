/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_OD4TOSTDOUT_HPP
#define CLUON_OD4TOSTDOUT_HPP

#include "cluon/cluon.hpp"
#include "cluon/Envelope.hpp"
#include "cluon/OD4Session.hpp"

#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

inline int32_t cluon_OD4toStdout(int32_t argc, char **argv) {
    int retVal{1};
    const std::string PROGRAM{argv[0]}; // NOLINT
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (0 == commandlineArguments.count("cid")) {
        std::cerr << PROGRAM
                  << " dumps Envelopes received from an OD4Session to stdout." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " --cid=<OpenDaVINCI session>" << std::endl;
        std::cerr << "Example: " << PROGRAM << " --cid=111" << std::endl;
    }
    else {
        // Interface to a running OpenDaVINCI session (ignoring any incoming Envelopes).
        cluon::OD4Session od4Session(static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])),
            [](cluon::data::Envelope &&envelope) noexcept {
                std::cout << cluon::serializeEnvelope(std::move(envelope));
                std::cout.flush();
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

