/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_TCPSERVER_HPP
#define CLUON_TCPSERVER_HPP

#include "cluon/TCPConnection.hpp"
#include "cluon/cluon.hpp"

// clang-format off
#ifdef WIN32
    #include <Winsock2.h> // for WSAStartUp
    #include <ws2tcpip.h> // for SOCKET
#else
    #include <netinet/in.h>
#endif
// clang-format on

#include <cstdint>
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

namespace cluon {

class LIBCLUON_API TCPServer {
   private:
    TCPServer(const TCPServer &) = delete;
    TCPServer(TCPServer &&)      = delete;
    TCPServer &operator=(const TCPServer &) = delete;
    TCPServer &operator=(TCPServer &&) = delete;

   public:
    /**
     * Constructor to create a TCP server.
     *
     * @param port Port to receive UDP packets from.
     * @param newConnectionDelegate Functional to handle incoming TCP connections.
     */
    TCPServer(uint16_t port, std::function<void(std::string &&from, std::shared_ptr<cluon::TCPConnection> connection)> newConnectionDelegate) noexcept;

    ~TCPServer() noexcept;

    /**
     * @return true if the TCPServer could successfully be created and is able to receive data.
     */
    bool isRunning() const noexcept;

   private:
    /**
     * This method closes the socket.
     *
     * @param errorCode Error code that caused this closing.
     */
    void closeSocket(int errorCode) noexcept;
    void readFromSocket() noexcept;

   private:
    mutable std::mutex m_socketMutex{};
    int32_t m_socket{-1};

    std::atomic<bool> m_readFromSocketThreadRunning{false};
    std::thread m_readFromSocketThread{};

    std::mutex m_newConnectionDelegateMutex{};
    std::function<void(std::string &&from, std::shared_ptr<cluon::TCPConnection> connection)> m_newConnectionDelegate{};
};
} // namespace cluon

#endif
