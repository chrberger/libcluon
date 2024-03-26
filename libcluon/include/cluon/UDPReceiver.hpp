/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_UDPRECEIVER_HPP
#define CLUON_UDPRECEIVER_HPP

#include "cluon/NotifyingPipeline.hpp"
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
#include <condition_variable>
#include <deque>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>

namespace cluon {
/**
To receive data from a UDP socket, simply include the header
`#include <cluon/UDPReceiver.hpp>`.

Next, create an instance of class `cluon::UDPReceiver` as follows:
`cluon::UDPReceiver receiver("127.0.0.1", 1234, delegate);`.
The first parameter is of type `std::string` expecting a numerical IPv4 address,
the second parameter specifies the UDP port, from which data shall be received
from, and the last parameter is of type `std::function` that is called whenever
new bytes are available to be processed.

The complete signature for the delegate function is
`std::function<void(std::string &&, std::string &&, std::chrono::system_clock::time_point &&) noexcept>`:
The first parameter contains the bytes that have been received, the second
parameter containes the human-readable representation of the sender
(X.Y.Z.W:ABCD), and the last parameter is the time stamp when the data has been
received. An example using a C++ lambda expression would look as follows:

\code{.cpp}
cluon::UDPReceiver receiver("127.0.0.1", 1234,
    [](std::string &&data, std::string &&sender, std::chrono::system_clock::time_point &&ts) noexcept {
        const auto timestamp(std::chrono::system_clock::to_time_t(ts));
        std::cout << "Received " << data.size() << " bytes"
                  << " from " << sender
                  << " at " << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %X")
                  << ", containing '" << data << "'." << std::endl;
    });
\endcode

After creating an instance of class `cluon::UDPReceiver`, it is immediately
activated and concurrently waiting for data in a separate thread. To check
whether the instance was created successfully and running, the method
`isRunning()` should be called.

A complete example is available
[here](https://github.com/chrberger/libcluon/blob/master/libcluon/examples/cluon-UDPReceiver.cpp).
*/
class LIBCLUON_API UDPReceiver {
   private:
    UDPReceiver(const UDPReceiver &) = delete;
    UDPReceiver(UDPReceiver &&)      = delete;
    UDPReceiver &operator=(const UDPReceiver &) = delete;
    UDPReceiver &operator=(UDPReceiver &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param receiveFromAddress Numerical IPv4 address to receive UDP packets from.
     * @param receiveFromPort Port to receive UDP packets from.
     * @param delegate Functional (noexcept) to handle received bytes; parameters are received data, sender, timestamp.
     * @param localSendFromPort Port that an application is using to send data. This port (> 0) is ignored when data is received.
     * @param interfaceAssociatedAddress Optional numerical IPv4 address associated with a interface. If given, will be used to
     * specify which interface to use when joining a multicast group.
     */
    UDPReceiver(const std::string &receiveFromAddress,
                uint16_t receiveFromPort,
                std::function<void(std::string &&, std::string &&, std::chrono::system_clock::time_point &&)> delegate,
                uint16_t localSendFromPort                    = 0,
                const std::string &interfaceAssociatedAddress = "") noexcept;
    ~UDPReceiver() noexcept;

    /**
     * @return true if the UDPReceiver could successfully be created and is able to receive data.
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
    int32_t m_socket{-1};
    bool m_isBlockingSocket{true};
    std::set<unsigned long> m_listOfLocalIPAddresses{};
    uint16_t m_localSendFromPort;
    struct sockaddr_in m_receiveFromAddress {};
    struct ip_mreq m_mreq {};
    bool m_isMulticast{false};

    std::atomic<bool> m_readFromSocketThreadRunning{false};
    std::thread m_readFromSocketThread{};

   private:
    std::function<void(std::string &&, std::string &&, std::chrono::system_clock::time_point)> m_delegate{};

   private:
    class PipelineEntry {
       public:
        std::string m_data;
        std::string m_from;
        std::chrono::system_clock::time_point m_sampleTime;
    };

    std::shared_ptr<cluon::NotifyingPipeline<PipelineEntry>> m_pipeline{};
};
} // namespace cluon

#endif
