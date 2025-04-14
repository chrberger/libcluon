/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_UDPSENDER_HPP
#define CLUON_UDPSENDER_HPP

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
#include <mutex>
#include <string>
#include <utility>

namespace cluon {
/**
To send data using a UDP socket, simply include the header
`#include <cluon/UDPSender.hpp>`.

Next, create an instance of class `cluon::UDPSender` as follows:
`cluon::UDPSender sender("127.0.0.1", 1234);`. The first parameter is of type
`std::string` expecting a numerical IPv4 address and the second parameter
specifies the UDP port to which the data shall be sent to.

To finally send data, simply call the method `send` supplying the data to be
sent: `sender.send(std::move("Hello World!")`. Please note that the data is
supplied using the _move_-semantics. The method `send` returns a
`std::pair<ssize_t, int32_t>` where the first element returns the size of the
successfully sent bytes and the second element contains the error code in case
the transmission of the data failed.

\code{.cpp}
cluon::UDPSender sender("127.0.0.1", 1234);

std::pair<ssize_t, int32_t> retVal = sender.send(std::move("Hello World!"));

std::cout << "Send " << retVal.first << " bytes, error code = " << retVal.second << std::endl;
\endcode

A complete example is available
[here](https://github.com/chrberger/libcluon/blob/master/libcluon/examples/cluon-UDPSender.cpp).
*/
class LIBCLUON_API UDPSender {
   private:
    UDPSender(const UDPSender &) = delete;
    UDPSender(UDPSender &&)      = delete;
    UDPSender &operator=(const UDPSender &) = delete;
    UDPSender &operator=(UDPSender &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param sendToAddress Numerical IPv4 address to send a UDP packet to.
     * @param sendToPort Port to send a UDP packet to.
     * @param interfaceAssociatedAddress Optional numerical IPv4 address associated with a interface. If given, will be used to
     * specify which interface to use when sending to a multicast group.
     */
    UDPSender(const std::string &sendToAddress, uint16_t sendToPort, const std::string &interfaceAssociatedAddress = "") noexcept;
    ~UDPSender() noexcept;

    /**
     * Send a given string.
     *
     * @param data Data to send.
     * @return Pair: Number of bytes sent and errno.
     */
    std::pair<ssize_t, int32_t> send(std::string &&data) const noexcept;

   public:
    /**
     * @return Port that this UDP sender will use for sending or 0 if no information available.
     */
    uint16_t getSendFromPort() const noexcept;

   private:
    mutable std::mutex m_socketMutex{};
    int32_t m_socket{-1};
    uint16_t m_portToSentFrom{0};
    struct sockaddr_in m_sendToAddress {};
};
} // namespace cluon

#endif
