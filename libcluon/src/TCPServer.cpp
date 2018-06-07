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

#include "cluon/TCPServer.hpp"
#include "cluon/TerminateHandler.hpp"

// clang-format off
#ifdef WIN32
    #include <errno.h>
    #include <iostream>
#else
    #include <arpa/inet.h>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif
// clang-format on

#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>

namespace cluon {

TCPServer::TCPServer(uint16_t port, std::function<void(std::string &&from, std::shared_ptr<cluon::TCPConnection> connection)> newConnectionDelegate) noexcept 
    : m_newConnectionDelegate(newConnectionDelegate) {
    if (0 < port) {
#ifdef WIN32
        // Load Winsock 2.2 DLL.
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "[cluon::TCPServer] Error while calling WSAStartUp: " << WSAGetLastError() << std::endl;
        }
#endif
        m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#ifdef WIN32
        if (m_socket < 0) {
            std::cerr << "[cluon::TCPServer] Error while creating socket: " << WSAGetLastError() << std::endl;
            WSACleanup();
        }
#endif

        if (!(m_socket < 0)) {
            // Allow reusing of ports by multiple calls with same address/port.
            uint32_t YES = 1;
            // clang-format off
            auto retVal = ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&YES), sizeof(YES)); // NOLINT
            // clang-format on
            if (0 > retVal) {
#ifdef WIN32 // LCOV_EXCL_LINE
                auto errorCode = WSAGetLastError();
#else
                auto errorCode = errno; // LCOV_EXCL_LINE
#endif                                  // LCOV_EXCL_LINE
                closeSocket(errorCode); // LCOV_EXCL_LINE
            }
        }


        if (!(m_socket < 0)) {
            // Setup address and port.
            struct sockaddr_in address;
            ::memset(&address, 0, sizeof(address));
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_ANY);
            address.sin_port = htons(port);

            auto retVal = ::bind(m_socket, reinterpret_cast<struct sockaddr*>(&address), sizeof(address));
            if (-1 != retVal) {
                constexpr int32_t MAX_PENDING_CONNECTIONS{100};
                retVal = ::listen(m_socket, MAX_PENDING_CONNECTIONS);
                if (-1 != retVal) {
                    // Constructing a thread could fail.
                    try {
                        m_readFromSocketThread = std::thread(&TCPServer::readFromSocket, this);

                        // Let the operating system spawn the thread.
                        using namespace std::literals::chrono_literals;
                        do { std::this_thread::sleep_for(1ms); } while (!m_readFromSocketThreadRunning.load());
                    } catch (...) { closeSocket(ECHILD); }
                }
                else {
#ifdef WIN32
                    auto errorCode = WSAGetLastError();
#else
                    auto errorCode = errno;
#endif
                    closeSocket(errorCode);
                }
            }
            else {
#ifdef WIN32
                auto errorCode = WSAGetLastError();
#else
                auto errorCode = errno;
#endif
                closeSocket(errorCode);
            }
        }
    }
}

TCPServer::~TCPServer() noexcept {
    m_readFromSocketThreadRunning.store(false);

    // Joining the thread could fail.
    try {
        if (m_readFromSocketThread.joinable()) {
            m_readFromSocketThread.join();
        }
    } catch (...) {}

    closeSocket(0);
}

void TCPServer::closeSocket(int errorCode) noexcept {
    if (0 != errorCode) {
        std::cerr << "[cluon::TCPServer] Failed to perform socket operation: ";
#ifdef WIN32
        std::cerr << errorCode << std::endl;
#else
        std::cerr << ::strerror(errorCode) << " (" << errorCode << ")" << std::endl;
#endif
    }

    if (!(m_socket < 0)) {
#ifdef WIN32
        ::shutdown(m_socket, SD_BOTH);
        ::closesocket(m_socket);
        WSACleanup();
#else
        ::shutdown(m_socket, SHUT_RDWR); // Disallow further read/write operations.
        ::close(m_socket);
#endif
    }
    m_socket = -1;
}

bool TCPServer::isRunning() const noexcept {
    return (m_readFromSocketThreadRunning.load() && !TerminateHandler::instance().isTerminated.load());
}

void TCPServer::readFromSocket() noexcept {
    struct timeval timeout {};

    // Define file descriptor set to watch for read operations.
    fd_set setOfFiledescriptorsToReadFrom{};

    // Indicate to main thread that we are ready.
    m_readFromSocketThreadRunning.store(true);

    constexpr uint16_t MAX_ADDR_SIZE{1024};
    std::array<char, MAX_ADDR_SIZE> remoteAddress{};

    while (m_readFromSocketThreadRunning.load()) {
        // Define timeout for select system call. The timeval struct must be
        // reinitialized for every select call as it might be modified containing
        // the actual time slept.
        timeout.tv_sec  = 0;
        timeout.tv_usec = 20 * 1000; // Check for new data with 50Hz.

        FD_ZERO(&setOfFiledescriptorsToReadFrom);
        FD_SET(m_socket, &setOfFiledescriptorsToReadFrom);
        ::select(m_socket + 1, &setOfFiledescriptorsToReadFrom, nullptr, nullptr, &timeout);
        if (FD_ISSET(m_socket, &setOfFiledescriptorsToReadFrom)) {
            struct sockaddr_storage remote;
            socklen_t addrLength = sizeof(remote);
            int32_t connectingClient = ::accept(m_socket, reinterpret_cast<struct sockaddr *>(&remote), &addrLength);
            if ( (0 <= connectingClient) && (nullptr != m_newConnectionDelegate) ) {
                ::inet_ntop(remote.ss_family,
                            &((reinterpret_cast<struct sockaddr_in *>(&remote))->sin_addr), // NOLINT
                            remoteAddress.data(),
                            remoteAddress.max_size());
                const uint16_t RECVFROM_PORT{ntohs(reinterpret_cast<struct sockaddr_in *>(&remote)->sin_port)}; // NOLINT
                m_newConnectionDelegate(std::string(remoteAddress.data()) + ':' + std::to_string(RECVFROM_PORT), std::shared_ptr<cluon::TCPConnection>(new cluon::TCPConnection(connectingClient)));
            }
        }
    }
}
} // namespace cluon
