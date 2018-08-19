/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
        cluon::TCPServer srv(static_cast<uint16_t>(std::stoi(PORT)), [&connections](std::string &&from, std::shared_ptr<cluon::TCPConnection> connection){
            std::cout << "Got connection from " << from << std::endl;
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
