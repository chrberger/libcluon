/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/UDPReceiver.hpp"
#include "cluon/IPv4Tools.hpp"
#include "cluon/TerminateHandler.hpp"
#include "cluon/UDPPacketSizeConstraints.hpp"

// clang-format off
#ifdef WIN32
    #include <cstdio>
    #include <cerrno>

    #include <winsock2.h>
    #include <iphlpapi.h>
    #include <ws2tcpip.h>

    #include <iostream>
#else
    #ifdef __linux__
        #include <linux/sockios.h>
    #endif

    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

#ifndef WIN32
    #include <ifaddrs.h>
    #include <netdb.h>
#endif
// clang-format on

#include <cstring>
#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

namespace cluon {

UDPReceiver::UDPReceiver(const std::string &receiveFromAddress,
                         uint16_t receiveFromPort,
                         std::function<void(std::string &&, std::string &&, std::chrono::system_clock::time_point &&)> delegate,
                         uint16_t localSendFromPort) noexcept
    : m_localSendFromPort(localSendFromPort)
    , m_receiveFromAddress()
    , m_mreq()
    , m_readFromSocketThread()
    , m_delegate(std::move(delegate)) {
    // Decompose given address string to check validity with numerical IPv4 address.
    std::string tmp{cluon::getIPv4FromHostname(receiveFromAddress)};
    std::replace(tmp.begin(), tmp.end(), '.', ' ');
    std::istringstream sstr{tmp};
    std::vector<int> receiveFromAddressTokens{std::istream_iterator<int>(sstr), std::istream_iterator<int>()};

    if ((!receiveFromAddress.empty()) && (4 == receiveFromAddressTokens.size())
        && !(std::end(receiveFromAddressTokens)
             != std::find_if(receiveFromAddressTokens.begin(), receiveFromAddressTokens.end(), [](int a) { return (a < 0) || (a > 255); }))
        && (0 < receiveFromPort)) {
        // Check for valid IP address.
        struct sockaddr_in tmpSocketAddress {};
        const bool isValid = (0 < ::inet_pton(AF_INET, receiveFromAddress.c_str(), &(tmpSocketAddress.sin_addr))) && (224 > receiveFromAddressTokens[0] || 255 == receiveFromAddressTokens[0]); // Accept regular IP addresses (ie., non-multicast addesses and the network-wide broadcast address 255.255.255.255.

        // Check for UDP multicast, i.e., IP address range [225.0.0.1 - 239.255.255.255].
        m_isMulticast = (((224 < receiveFromAddressTokens[0]) && (receiveFromAddressTokens[0] <= 239))
                         && ((0 <= receiveFromAddressTokens[1]) && (receiveFromAddressTokens[1] <= 255))
                         && ((0 <= receiveFromAddressTokens[2]) && (receiveFromAddressTokens[2] <= 255))
                         && ((1 <= receiveFromAddressTokens[3]) && (receiveFromAddressTokens[3] <= 255)));

#ifndef WIN32
        // Check whether given address is a broadcast address.
        bool isBroadcast{false};
        {
            isBroadcast |= (receiveFromAddress == "255.255.255.255");
            if (!isBroadcast) {
                struct ifaddrs *ifaddr{nullptr};
                if (0 == getifaddrs(&ifaddr)) {
                    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                        if (ifa->ifa_addr == NULL) {
                            continue;
                        }

                        if (ifa->ifa_addr->sa_family == AF_INET) {
                            char broadcastAddress[NI_MAXHOST];
#ifdef __APPLE__
                            if (0 == getnameinfo(ifa->ifa_dstaddr,
                                   sizeof(struct sockaddr_in),
                                   broadcastAddress, NI_MAXHOST,
                                   NULL, 0, NI_NUMERICHOST))
#else
                            if (0 == getnameinfo(ifa->ifa_ifu.ifu_broadaddr,
                                   sizeof(struct sockaddr_in),
                                   broadcastAddress, NI_MAXHOST,
                                   NULL, 0, NI_NUMERICHOST))
#endif
                            {
                                 std::string _tmp{broadcastAddress};
                                 isBroadcast |= (_tmp.compare(receiveFromAddress) == 0);
                            }
                        }
                    }
                    freeifaddrs(ifaddr);
                }
            }
        }
#endif

        std::memset(&m_receiveFromAddress, 0, sizeof(m_receiveFromAddress));
#ifdef WIN32
        // According to http://www.sockets.com/err_lst1.htm, the binding is
        // different on Windows opposed to POSIX when using the real address
        // here; thus, we need to use INADDR_ANY.
        m_receiveFromAddress.sin_addr.s_addr = (m_isMulticast ? htonl(INADDR_ANY) : ::inet_addr(receiveFromAddress.c_str()));
#else
        m_receiveFromAddress.sin_addr.s_addr = ::inet_addr(receiveFromAddress.c_str());
#endif
        m_receiveFromAddress.sin_family = AF_INET;
        m_receiveFromAddress.sin_port   = htons(receiveFromPort);

#ifdef WIN32
        // Load Winsock 2.2 DLL.
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "[cluon::UDPReceiver] Error while calling WSAStartUp: " << WSAGetLastError() << std::endl;
        }
#endif

        m_socket = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

#ifdef WIN32
        if (m_socket < 0) {
            std::cerr << "[cluon::UDPReceiver] Error while creating socket: " << WSAGetLastError() << std::endl;
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

#ifndef WIN32
        if (!(m_socket < 0) && isBroadcast) {
            // Enabling broadcast.
            uint32_t YES = 1;
            // clang-format off
            auto retVal = ::setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&YES), sizeof(YES)); // NOLINT
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
#endif

        if (!(m_socket < 0)) {
            // Trying to enable non_blocking mode.
#ifdef WIN32 // LCOV_EXCL_LINE
            u_long nonBlocking = 1;
            m_isBlockingSocket = !(NO_ERROR == ::ioctlsocket(m_socket, FIONBIO, &nonBlocking));
#else
            const int FLAGS    = ::fcntl(m_socket, F_GETFL, 0);
            m_isBlockingSocket = !(0 == ::fcntl(m_socket, F_SETFL, FLAGS | O_NONBLOCK));
#endif
        }

        if (!(m_socket < 0)) {
            // Trying to enable non_blocking mode.
#ifdef WIN32 // LCOV_EXCL_LINE
            u_long nonBlocking = 1;
            m_isBlockingSocket = !(NO_ERROR == ::ioctlsocket(m_socket, FIONBIO, &nonBlocking));
#else
            const int FLAGS    = ::fcntl(m_socket, F_GETFL, 0);
            m_isBlockingSocket = !(0 == ::fcntl(m_socket, F_SETFL, FLAGS | O_NONBLOCK));
#endif
        }

        if (!(m_socket < 0)) {
            // Try setting receiving buffer.
            int recvBuffer{26214400};
            auto retVal = ::setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&recvBuffer), sizeof(recvBuffer));
            if (retVal < 0) {
#ifdef WIN32 // LCOV_EXCL_LINE
                auto errorCode = WSAGetLastError();
#else
                auto errorCode = errno; // LCOV_EXCL_LINE
#endif                                                                                                                                       // LCOV_EXCL_LINE
                std::cerr << "[cluon::UDPReceiver] Error while trying to set SO_RCVBUF to " << recvBuffer << ": " << errorCode << std::endl; // LCOV_EXCL_LINE
            }
        }

        if (!(m_socket < 0)) {
            // Bind to receive address/port.
            // clang-format off
            auto retVal = ::bind(m_socket, reinterpret_cast<struct sockaddr *>(&m_receiveFromAddress), sizeof(m_receiveFromAddress)); // NOLINT
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
            if (m_isMulticast) {
                // Join the multicast group.
                m_mreq.imr_multiaddr.s_addr = ::inet_addr(receiveFromAddress.c_str());
                m_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
                // clang-format off
                auto retval                 = ::setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char *>(&m_mreq), sizeof(m_mreq)); // NOLINT
                // clang-format on
                if (0 > retval) { // LCOV_EXCL_LINE
#ifdef WIN32                      // LCOV_EXCL_LINE
                    closeSocket(WSAGetLastError());
#else
                    closeSocket(errno); // LCOV_EXCL_LINE
#endif // LCOV_EXCL_LINE
                }
            } else if (!isValid) {
                closeSocket(EBADF);
            }
        }

        // Fill list of local IP address to avoid sending data to ourselves.
        if (!(m_socket < 0)) {
#ifdef WIN32
            DWORD size{0};
            if (ERROR_BUFFER_OVERFLOW == GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size)) {
                PIP_ADAPTER_ADDRESSES adapters = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(malloc(size));
                if (ERROR_SUCCESS == GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapters, &size)) {
                    for (PIP_ADAPTER_ADDRESSES adapter = adapters; nullptr != adapter; adapter = adapter->Next) {
                        for (PIP_ADAPTER_UNICAST_ADDRESS unicastAddress = adapter->FirstUnicastAddress; unicastAddress != NULL;
                             unicastAddress                             = unicastAddress->Next) {
                            if (AF_INET == unicastAddress->Address.lpSockaddr->sa_family) {
                                ::getnameinfo(unicastAddress->Address.lpSockaddr, unicastAddress->Address.iSockaddrLength, nullptr, 0, NULL, 0, NI_NUMERICHOST);
                                std::memcpy(&tmpSocketAddress, unicastAddress->Address.lpSockaddr, sizeof(tmpSocketAddress)); /* Flawfinder: ignore */ // NOLINT
                                const unsigned long LOCAL_IP = tmpSocketAddress.sin_addr.s_addr;
                                m_listOfLocalIPAddresses.insert(LOCAL_IP);
                            }
                        }
                    }
                }
                free(adapters);
            }
#else
            struct ifaddrs *interfaceAddress;
            if (0 == ::getifaddrs(&interfaceAddress)) {
                for (struct ifaddrs *it = interfaceAddress; nullptr != it; it = it->ifa_next) {
                    if ((nullptr != it->ifa_addr) && (it->ifa_addr->sa_family == AF_INET)) {
                        if (0 == ::getnameinfo(it->ifa_addr, sizeof(struct sockaddr_in), nullptr, 0, nullptr, 0, NI_NUMERICHOST)) {
                            std::memcpy(&tmpSocketAddress, it->ifa_addr, sizeof(tmpSocketAddress)); /* Flawfinder: ignore */ // NOLINT
                            const unsigned long LOCAL_IP = tmpSocketAddress.sin_addr.s_addr;
                            m_listOfLocalIPAddresses.insert(LOCAL_IP);
                        }
                    }
                }
                ::freeifaddrs(interfaceAddress);
            }
#endif
        }

        if (!(m_socket < 0)) {
            // Constructing the receiving thread could fail.
            try {
                m_readFromSocketThread = std::thread(&UDPReceiver::readFromSocket, this);

                // Let the operating system spawn the thread.
                using namespace std::literals::chrono_literals; // NOLINT
                do { std::this_thread::sleep_for(1ms); } while (!m_readFromSocketThreadRunning.load());
            } catch (...) { closeSocket(ECHILD); } // LCOV_EXCL_LINE

            try {
                m_pipeline = std::make_shared<cluon::NotifyingPipeline<PipelineEntry>>(
                    [this](PipelineEntry &&entry) { this->m_delegate(std::move(entry.m_data), std::move(entry.m_from), std::move(entry.m_sampleTime)); });
                if (m_pipeline) {
                    // Let the operating system spawn the thread.
                    using namespace std::literals::chrono_literals; // NOLINT
                    do { std::this_thread::sleep_for(1ms); } while (!m_pipeline->isRunning());
                }
            } catch (...) { closeSocket(ECHILD); } // LCOV_EXCL_LINE
        }
    }
}

UDPReceiver::~UDPReceiver() noexcept {
    {
        m_readFromSocketThreadRunning.store(false);

        // Joining the thread could fail.
        try {
            if (m_readFromSocketThread.joinable()) {
                m_readFromSocketThread.join();
            }
        } catch (...) {} // LCOV_EXCL_LINE
    }

    m_pipeline.reset();

    closeSocket(0);
}

void UDPReceiver::closeSocket(int errorCode) noexcept {
    if (0 != errorCode) {
        std::cerr << "[cluon::UDPReceiver] Failed to perform socket operation: ";
#ifdef WIN32
        std::cerr << errorCode << std::endl;
#else
        std::cerr << ::strerror(errorCode) << " (" << errorCode << ")" << std::endl;
#endif
    }

    if (!(m_socket < 0)) {
        if (m_isMulticast) {
            // clang-format off
            auto retVal = ::setsockopt(m_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, reinterpret_cast<char *>(&m_mreq), sizeof(m_mreq)); // NOLINT
            // clang-format on
            if (0 > retVal) {                                                                         // LCOV_EXCL_LINE
                std::cerr << "[cluon::UDPReceiver] Failed to drop multicast membership" << std::endl; // LCOV_EXCL_LINE
            }
        }

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

bool UDPReceiver::isRunning() const noexcept {
    return (m_readFromSocketThreadRunning.load() && !TerminateHandler::instance().isTerminated.load());
}

void UDPReceiver::readFromSocket() noexcept {
    // Create buffer to store data from socket.
    constexpr uint16_t MAX_LENGTH = static_cast<uint16_t>(UDPPacketSizeConstraints::MAX_SIZE_UDP_PACKET)
                                    - static_cast<uint16_t>(UDPPacketSizeConstraints::SIZE_IPv4_HEADER)
                                    - static_cast<uint16_t>(UDPPacketSizeConstraints::SIZE_UDP_HEADER);
    std::array<char, MAX_LENGTH> buffer{};

    struct timeval timeout {};

    // Define file descriptor set to watch for read operations.
    fd_set setOfFiledescriptorsToReadFrom{};

    // Sender address and port.
    constexpr uint16_t MAX_ADDR_SIZE{1024};
    std::array<char, MAX_ADDR_SIZE> remoteAddress{};

    struct sockaddr_storage remote {};
    socklen_t addrLength{sizeof(remote)};

    // Indicate to main thread that we are ready.
    m_readFromSocketThreadRunning.store(true);

    while (m_readFromSocketThreadRunning.load()) {
        // Define timeout for select system call. The timeval struct must be
        // reinitialized for every select call as it might be modified containing
        // the actual time slept.
        timeout.tv_sec  = 0;
        timeout.tv_usec = 20 * 1000; // Check for new data with 50Hz.

        FD_ZERO(&setOfFiledescriptorsToReadFrom);          // NOLINT
        FD_SET(m_socket, &setOfFiledescriptorsToReadFrom); // NOLINT
        ::select(m_socket + 1, &setOfFiledescriptorsToReadFrom, nullptr, nullptr, &timeout);

        ssize_t totalBytesRead{0};
        if (FD_ISSET(m_socket, &setOfFiledescriptorsToReadFrom)) { // NOLINT
            ssize_t bytesRead{0};
            do {
                bytesRead = ::recvfrom(m_socket,
                                       buffer.data(),
                                       buffer.max_size(),
                                       0,
                                       reinterpret_cast<struct sockaddr *>(&remote), // NOLINT
                                       reinterpret_cast<socklen_t *>(&addrLength));  // NOLINT

                if ((0 < bytesRead) && (nullptr != m_delegate)) {
#ifdef __linux__
                    std::chrono::system_clock::time_point timestamp;
                    struct timeval receivedTimeStamp {};
                    if (0 == ::ioctl(m_socket, SIOCGSTAMP, &receivedTimeStamp)) { // NOLINT
                        // Transform struct timeval to C++ chrono.
                        std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> transformedTimePoint(
                            std::chrono::microseconds(receivedTimeStamp.tv_sec * 1000000L + receivedTimeStamp.tv_usec));
                        timestamp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(transformedTimePoint);
                    } else { // LCOV_EXCL_LINE
                        // In case the ioctl failed, fall back to chrono. // LCOV_EXCL_LINE
                        timestamp = std::chrono::system_clock::now(); // LCOV_EXCL_LINE
                    }
#else
                    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
#endif

                    // Transform sender address to C-string.
                    ::inet_ntop(remote.ss_family,
                                &((reinterpret_cast<struct sockaddr_in *>(&remote))->sin_addr), // NOLINT
                                remoteAddress.data(),
                                remoteAddress.max_size());
                    const unsigned long RECVFROM_IP{reinterpret_cast<struct sockaddr_in *>(&remote)->sin_addr.s_addr}; // NOLINT
                    const uint16_t RECVFROM_PORT{ntohs(reinterpret_cast<struct sockaddr_in *>(&remote)->sin_port)};    // NOLINT

                    // Check if the bytes actually came from us.
                    bool sentFromUs{false};
                    {
                        auto pos                   = m_listOfLocalIPAddresses.find(RECVFROM_IP);
                        const bool sentFromLocalIP = (pos != m_listOfLocalIPAddresses.end() && (*pos == RECVFROM_IP));
                        sentFromUs                 = sentFromLocalIP && (m_localSendFromPort == RECVFROM_PORT);
                    }

                    // Create a pipeline entry to be processed concurrently.
                    if (!sentFromUs) {
                        PipelineEntry pe;
                        pe.m_data       = std::string(buffer.data(), static_cast<size_t>(bytesRead));
                        pe.m_from       = std::string(remoteAddress.data()) + ':' + std::to_string(RECVFROM_PORT);
                        pe.m_sampleTime = timestamp;

                        // Store entry in queue.
                        if (m_pipeline) {
                            m_pipeline->add(std::move(pe));
                        }
                    }
                    totalBytesRead += bytesRead;
                }
            } while (!m_isBlockingSocket && (bytesRead > 0));
        }

        if (static_cast<int32_t>(totalBytesRead) > 0) {
            if (m_pipeline) {
                m_pipeline->notifyAll();
            }
        }
    }
}
} // namespace cluon
