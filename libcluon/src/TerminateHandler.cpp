/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/TerminateHandler.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace cluon {

inline void cluon_handleExit() {
    TerminateHandler::instance().isTerminated.store(true);
}

inline void cluon_handleSignal(int32_t /*signal*/) {       // LCOV_EXCL_LINE
    TerminateHandler::instance().isTerminated.store(true); // LCOV_EXCL_LINE
}

TerminateHandler::TerminateHandler() noexcept {
    if (0 != std::atexit(cluon_handleExit)) {
        std::cerr << "[cluon::TerminateHandler] Failed to register cluon_exitHandler()." << std::endl; // LCOV_EXCL_LINE
    }

#ifdef WIN32
    if (SIG_ERR == ::signal(SIGINT, &cluon_handleSignal)) {
        std::cerr << "[cluon::TerminateHandler] Failed to register signal SIGINT." << std::endl;
    }
    if (SIG_ERR == ::signal(SIGTERM, &cluon_handleSignal)) {
        std::cerr << "[cluon::TerminateHandler] Failed to register signal SIGTERM." << std::endl;
    }
#else
    std::memset(&m_signalHandler, 0, sizeof(m_signalHandler));
    m_signalHandler.sa_handler = &cluon_handleSignal;

    if (::sigaction(SIGINT, &m_signalHandler, NULL) < 0) {
        std::cerr << "[cluon::TerminateHandler] Failed to register signal SIGINT." << std::endl; // LCOV_EXCL_LINE
    }
    if (::sigaction(SIGTERM, &m_signalHandler, NULL) < 0) {
        std::cerr << "[cluon::TerminateHandler] Failed to register signal SIGTERM." << std::endl; // LCOV_EXCL_LINE
    }
#endif
}

} // namespace cluon
