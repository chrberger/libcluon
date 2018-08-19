/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/UDPSender.hpp"

#include <iostream>
#include <string>

int main(int argc, char **argv) {
    int retVal{0};
    const std::string PROGRAM(argv[0]); //NOLINT
    if (3 != argc) {
        std::cerr << PROGRAM << " demonstrates how to use libcluon to send data via UDP." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " IPv4-address port" << std::endl;
        std::cerr << "Example: " << PROGRAM << " 127.0.0.1 1234" << std::endl;
        retVal = 1;
    } else {
        const std::string ADDRESS(argv[1]); //NOLINT
        const std::string PORT(argv[2]); //NOLINT
        cluon::UDPSender sender(ADDRESS, static_cast<uint16_t>(std::stoi(PORT)));

        while (std::cin.good()) {
            std::string data;
            std::getline(std::cin, data);

            sender.send(std::move(data));
        }
    }
    return retVal;
}
