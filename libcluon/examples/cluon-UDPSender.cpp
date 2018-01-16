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
        cluon::UDPSender sender(ADDRESS, static_cast<const uint16_t>(std::stoi(PORT)));

        while (std::cin.good()) {
            std::string data;
            std::getline(std::cin, data);

            sender.send(std::move(data));
        }
    }
    return retVal;
}
