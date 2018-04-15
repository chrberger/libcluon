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

#ifndef CLUON_TCPCONNECTION_HPP
#define CLUON_TCPCONNECTION_HPP

#include "cluon/cluon.hpp"

// clang-format off
#ifdef WIN32
    #include <Winsock2.h> // for WSAStartUp
    #include <Ws2def.h>   // for struct sockaddr_in
    #include <ws2tcpip.h> // for SOCKET
#else
    #include <netinet/in.h>
#endif
// clang-format on

#include <cstdint>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

namespace cluon {
/**
To exchange data via TCP, simply include the header
`#include <cluon/TCPConnection.hpp>`.

Next, create an instance of class `cluon::TCPConnection` as follows:
`cluon::TCPConnection connection("127.0.0.1", 1234, newDataDelegate, connectionLostDelegate);`.
The first parameter is of type `std::string` expecting a numerical IPv4 address,
the second parameter specifies the TCP port, from which data shall be received
from, the third paraemter is of type `std::function` that is called whenever
new bytes are available to be processed, and the last parameter is of type
`std::function` that is called when the connection is lost.

The complete signature for the newDataDelegate function is
`std::function<void(std::string &&, std::string &&, std::chrono::system_clock::time_point &&) noexcept>`:
The first parameter contains the bytes that have been received, the second
parameter containes the human-readable representation of the sender
(X.Y.Z.W:ABCD), and the last parameter is the time stamp when the data has been
received.

The complete signature for the connectionLostDelegate function is
`std::function<void() noexcept>`.

To finally send data, simply call the method `send` supplying the data to be
sent: `connection.send(std::move("Hello World!")`. Please note that the data is
supplied using the _move_-semantics. The method `send` returns a
`std::pair<ssize_t, int32_t>` where the first element returns the size of the
successfully sent bytes and the second element contains the error code in case
the transmission of the data failed.

An example using a C++ lambda expression would look as follows:

\code{.cpp}
cluon::TCPConnection connection("127.0.0.1", 1234,
    [](std::string &&data, std::string &&sender, std::chrono::system_clock::time_point &&ts) noexcept {
        const auto timestamp(std::chrono::system_clock::to_time_t(ts));
        std::cout << "Received " << data.size() << " bytes"
                  << " from " << sender
                  << " at " << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %X")
                  << ", containing '" << data << "'." << std::endl;
    },
    [](){ std::cout << "Connection lost." << std::endl; });

std::pair<ssize_t, int32_t> retVal = connection.send(std::move("Hello World!"));
\endcode

After creating an instance of class `cluon::TCPConnection`, it is immediately
activated and concurrently waiting for data in a separate thread. To check
whether the instance was created successfully and running, the method
`isRunning()` should be called.
*/
class LIBCLUON_API TCPConnection {
   private:
    TCPConnection(const TCPConnection &) = delete;
    TCPConnection(TCPConnection &&)      = delete;
    TCPConnection &operator=(const TCPConnection &) = delete;
    TCPConnection &operator=(TCPConnection &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param address Numerical IPv4 address to receive UDP packets from.
     * @param port Port to receive UDP packets from.
     * @param newDataDelegate Functional (noexcept) to handle received bytes; parameters are received data, timestamp.
     * @param connectionLostDelegate Functional (noexcept) to handle a lost connection.
     */
    TCPConnection(const std::string &address,
                  uint16_t port,
                  std::function<void(std::string &&, std::chrono::system_clock::time_point &&)> newDataDelegate,
                  std::function<void()> connectionLostDelegate) noexcept;
    ~TCPConnection() noexcept;

    /**
     * @return true if the TCPConnection could successfully be created and is able to receive data.
     */
    bool isRunning() const noexcept;

    /**
     * Send a given string.
     *
     * @param data Data to send.
     * @return Pair: Number of bytes sent and errno.
     */
    std::pair<ssize_t, int32_t> send(std::string &&data) const noexcept;

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
    struct sockaddr_in m_address {};
    std::atomic<bool> m_readFromSocketThreadRunning{false};
    std::thread m_readFromSocketThread{};
    std::function<void(std::string &&, std::chrono::system_clock::time_point)> m_newDataDelegate{};
    std::function<void()> m_connectionLostDelegate{};
};
} // namespace cluon

#endif
