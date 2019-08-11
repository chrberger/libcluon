/*
 * Copyright (C) 2019  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/IPv4Tools.hpp"

#ifndef WIN32
    #include <arpa/inet.h>
    #include <netdb.h>

    #if defined(BSD)
        #include <netinet/in.h>
        #include <sys/socket.h>
    #endif
#endif

#include <cstring>

namespace cluon {

std::string getIPv4FromHostname(const std::string &hostname) noexcept {
    std::string result{""};
    if (!hostname.empty()) {
        struct addrinfo hint;
        {
            std::memset(&hint, sizeof(struct addrinfo), 1);
            hint.ai_flags = AI_CANONNAME;
            hint.ai_family = AF_INET;
            hint.ai_socktype = 0;
            hint.ai_protocol = 0;
            hint.ai_addrlen = 0;
            hint.ai_canonname = nullptr;
            hint.ai_addr = nullptr;
            hint.ai_next = nullptr;
        }

        struct addrinfo *listOfHosts{nullptr};
        if (0 == getaddrinfo(hostname.c_str(), nullptr, &hint, &listOfHosts)) {
            for(struct addrinfo *e = listOfHosts; nullptr != listOfHosts; listOfHosts = listOfHosts->ai_next) {
                if (nullptr != e) {
                    if (AF_INET == e->ai_family) {
                        struct sockaddr_in *sinp = reinterpret_cast<struct sockaddr_in*>(e->ai_addr);
                        char buf[INET_ADDRSTRLEN];
                        const char *addr = inet_ntop(AF_INET, &sinp->sin_addr, buf, INET_ADDRSTRLEN);
                        if ( (nullptr != addr) && (result.empty()) ) {
                            result = std::string(addr);
                            break;
                        }
                    }
                }
            }
        }

        if (nullptr != listOfHosts) {
            freeaddrinfo(listOfHosts);
        }
    }
    return result;
}

} // namespace cluon
