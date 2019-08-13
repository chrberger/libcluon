/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/TCPConnection.hpp"
#include "cluon/IPv4Tools.hpp"
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
#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

namespace cluon {

TCPConnection::TCPConnection(const int32_t &socket) noexcept
    : m_socket(socket)
    , m_newDataDelegate(nullptr)
    , m_connectionLostDelegate(nullptr) {
    if (!(m_socket < 0)) {
        startReadingFromSocket();
    }
}

TCPConnection::TCPConnection(const std::string &address,
                             uint16_t port,
                             std::function<void(std::string &&, std::chrono::system_clock::time_point &&)> newDataDelegate,
                             std::function<void()> connectionLostDelegate) noexcept
    : m_newDataDelegate(std::move(newDataDelegate))
    , m_connectionLostDelegate(std::move(connectionLostDelegate)) {
    // Decompose given address string to check validity with numerical IPv4 address.
    std::string resolvedHostname{cluon::getIPv4FromHostname(address)};
    std::string tmp{resolvedHostname};
    std::replace(tmp.begin(), tmp.end(), '.', ' ');
    std::istringstream sstr{tmp};
    std::vector<int> addressTokens{std::istream_iterator<int>(sstr), std::istream_iterator<int>()};

    if ((!addressTokens.empty()) && (4 == addressTokens.size())
        && !(std::end(addressTokens) != std::find_if(addressTokens.begin(), addressTokens.end(), [](int a) { return (a < 0) || (a > 255); })) && (0 < port)) {
        // Check for valid IP address.
        struct sockaddr_in tmpSocketAddress {};
        const bool isValid = (0 < ::inet_pton(AF_INET, resolvedHostname.c_str(), &(tmpSocketAddress.sin_addr)));
        if (isValid) {
            std::memset(&m_address, 0, sizeof(m_address));
            m_address.sin_addr.s_addr = ::inet_addr(resolvedHostname.c_str());
            m_address.sin_family      = AF_INET;
            m_address.sin_port        = htons(port);
#ifdef WIN32
            // Load Winsock 2.2 DLL.
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "[cluon::TCPConnection] Error while calling WSAStartUp: " << WSAGetLastError() << std::endl;
            }
#endif

            m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#ifdef WIN32
            if (m_socket < 0) {
                std::cerr << "[cluon::TCPConnection] Error while creating socket: " << WSAGetLastError() << std::endl;
                WSACleanup();
            }
#endif

            if (!(m_socket < 0)) {
                auto retVal = ::connect(m_socket, reinterpret_cast<struct sockaddr *>(&m_address), sizeof(m_address));
                if (0 > retVal) {
#ifdef WIN32 // LCOV_EXCL_LINE
                    auto errorCode = WSAGetLastError();
#else
                    auto errorCode = errno;                                          // LCOV_EXCL_LINE
#endif                                      // LCOV_EXCL_LINE
                    closeSocket(errorCode); // LCOV_EXCL_LINE
                } else {
                    startReadingFromSocket();
                }
            }
        }
    }
}

TCPConnection::~TCPConnection() noexcept {
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

void TCPConnection::closeSocket(int errorCode) noexcept {
    if (0 != errorCode) {
        std::cerr << "[cluon::TCPConnection] Failed to perform socket operation: "; // LCOV_EXCL_LINE
#ifdef WIN32                                                                        // LCOV_EXCL_LINE
        std::cerr << errorCode << std::endl;
#else
        std::cerr << ::strerror(errorCode) << " (" << errorCode << ")" << std::endl; // LCOV_EXCL_LINE
#endif // LCOV_EXCL_LINE
    }

    if (!(m_socket < 0)) {
#ifdef WIN32
        ::shutdown(m_socket, SD_BOTH);
        ::closesocket(m_socket);
        WSACleanup();
#else
        ::shutdown(m_socket, SHUT_RDWR);                                             // Disallow further read/write operations.
        ::close(m_socket);
#endif
    }
    m_socket = -1;
}

void TCPConnection::startReadingFromSocket() noexcept {
    // Constructing a thread could fail.
    try {
        m_readFromSocketThread = std::thread(&TCPConnection::readFromSocket, this);

        // Let the operating system spawn the thread.
        using namespace std::literals::chrono_literals;
        do { std::this_thread::sleep_for(1ms); } while (!m_readFromSocketThreadRunning.load());
    } catch (...) {          // LCOV_EXCL_LINE
        closeSocket(ECHILD); // LCOV_EXCL_LINE
    }

    try {
        m_pipeline = std::make_shared<cluon::NotifyingPipeline<PipelineEntry>>(
            [this](PipelineEntry &&entry) { this->m_newDataDelegate(std::move(entry.m_data), std::move(entry.m_sampleTime)); });
        if (m_pipeline) {
            // Let the operating system spawn the thread.
            using namespace std::literals::chrono_literals; // NOLINT
            do { std::this_thread::sleep_for(1ms); } while (!m_pipeline->isRunning());
        }
    } catch (...) { closeSocket(ECHILD); } // LCOV_EXCL_LINE
}

void TCPConnection::setOnNewData(std::function<void(std::string &&, std::chrono::system_clock::time_point &&)> newDataDelegate) noexcept {
    std::lock_guard<std::mutex> lck(m_newDataDelegateMutex);
    m_newDataDelegate = newDataDelegate;
}

void TCPConnection::setOnConnectionLost(std::function<void()> connectionLostDelegate) noexcept {
    std::lock_guard<std::mutex> lck(m_connectionLostDelegateMutex);
    m_connectionLostDelegate = connectionLostDelegate;
}

bool TCPConnection::isRunning() const noexcept {
    return (m_readFromSocketThreadRunning.load() && !TerminateHandler::instance().isTerminated.load());
}

std::pair<ssize_t, int32_t> TCPConnection::send(std::string &&data) const noexcept {
    if (-1 == m_socket) {
        return {-1, EBADF};
    }

    if (data.empty()) {
        return {0, 0};
    }

    if (!m_readFromSocketThreadRunning.load()) {
        std::lock_guard<std::mutex> lck(m_connectionLostDelegateMutex); // LCOV_EXCL_LINE
        if (nullptr != m_connectionLostDelegate) {                      // LCOV_EXCL_LINE
            m_connectionLostDelegate();                                 // LCOV_EXCL_LINE
        }
        return {-1, ENOTCONN}; // LCOV_EXCL_LINE
    }

    constexpr uint16_t MAX_LENGTH{65535};
    if (MAX_LENGTH < data.size()) {
        return {-1, E2BIG};
    }

    std::lock_guard<std::mutex> lck(m_socketMutex);
    ssize_t bytesSent = ::send(m_socket, data.c_str(), data.length(), 0);
    return {bytesSent, (0 > bytesSent ? errno : 0)};
}

void TCPConnection::readFromSocket() noexcept {
    // Create buffer to store data from socket.
    constexpr uint16_t MAX_LENGTH{65535};
    std::array<char, MAX_LENGTH> buffer{};

    struct timeval timeout {};

    // Define file descriptor set to watch for read operations.
    fd_set setOfFiledescriptorsToReadFrom{};

    // Indicate to main thread that we are ready.
    m_readFromSocketThreadRunning.store(true);

    // This flag is used to not read data from the socket until this TCPConnection has a proper onNewDataHandler set.
    bool hasNewDataDelegate{false};

    while (m_readFromSocketThreadRunning.load()) {
        // Define timeout for select system call. The timeval struct must be
        // reinitialized for every select call as it might be modified containing
        // the actual time slept.
        timeout.tv_sec  = 0;
        timeout.tv_usec = 20 * 1000; // Check for new data with 50Hz.

        FD_ZERO(&setOfFiledescriptorsToReadFrom);
        FD_SET(m_socket, &setOfFiledescriptorsToReadFrom);
        ::select(m_socket + 1, &setOfFiledescriptorsToReadFrom, nullptr, nullptr, &timeout);

        // Only read data when the newDataDelegate is set.
        if (!hasNewDataDelegate) {
            std::lock_guard<std::mutex> lck(m_newDataDelegateMutex);
            hasNewDataDelegate = (nullptr != m_newDataDelegate);
        }
        if (FD_ISSET(m_socket, &setOfFiledescriptorsToReadFrom) && hasNewDataDelegate) {
            ssize_t bytesRead = ::recv(m_socket, buffer.data(), buffer.max_size(), 0);
            if (0 >= bytesRead) {
                // 0 == bytesRead: peer shut down the connection; 0 > bytesRead: other error.
                m_readFromSocketThreadRunning.store(false);

                {
                    std::lock_guard<std::mutex> lck(m_connectionLostDelegateMutex);
                    if (nullptr != m_connectionLostDelegate) {
                        m_connectionLostDelegate();
                    }
                }
                break;
            }

            {
                std::lock_guard<std::mutex> lck(m_newDataDelegateMutex);
                if ((0 < bytesRead) && (nullptr != m_newDataDelegate)) {
                    // SIOCGSTAMP is not available for a stream-based socket,
                    // thus, falling back to regular chrono timestamping.
                    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
                    {
                        PipelineEntry pe;
                        pe.m_data       = std::string(buffer.data(), static_cast<size_t>(bytesRead));
                        pe.m_sampleTime = timestamp;

                        // Store entry in queue.
                        if (m_pipeline) {
                            m_pipeline->add(std::move(pe));
                        }
                    }

                    if (m_pipeline) {
                        m_pipeline->notifyAll();
                    }
                }
            }
        }
    }
}
} // namespace cluon
