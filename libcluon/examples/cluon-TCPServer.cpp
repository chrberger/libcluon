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

#ifdef WIN32
// Disable deprecated API warnings.
#pragma warning(disable : 4996)
#endif

#include "cluon/TCPConnection.hpp"
#include "cluon/TCPServer.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

int main(int argc, char **argv) {
    int retVal{1};
    const std::string PROGRAM(argv[0]); // NOLINT
    if (2 != argc) {
        std::cerr << PROGRAM
                  << " demonstrates how to use libcluon to send & receive data as a TCP server (running for 60s)." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " port" << std::endl;
        std::cerr << "Example: " << PROGRAM << " 1234" << std::endl;
    } else {
        const std::string PORT(argv[1]); // NOLINT
        std::vector<std::shared_ptr<cluon::TCPConnection> > connections;
        cluon::TCPServer srv(static_cast<uint16_t>(std::stoi(PORT)), [&connections](std::shared_ptr<cluon::TCPConnection> connection){
            std::cout << "Got connection..." << std::endl;
            connection->setOnNewData([](std::string &&data, std::chrono::system_clock::time_point &&){
                std::cout << "Data: '" << data << "'" << std::endl;
            });
            connection->setOnConnectionLost([](){
                std::cout << "Connection lost." << std::endl;
            });
            connections.push_back(connection);
        });

        int counter{0};
        while (srv.isRunning() && (60 > counter++)) {
            using namespace std::literals::chrono_literals; // NOLINT
            std::this_thread::sleep_for(1s);
        }
        retVal = 0;
    }
    return retVal;
}
