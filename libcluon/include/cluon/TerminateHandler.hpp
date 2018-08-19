/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_TERMINATEHANDLER_HPP
#define CLUON_TERMINATEHANDLER_HPP

#include "cluon/cluon.hpp"

#include <atomic>
#include <csignal>

namespace cluon {

class LIBCLUON_API TerminateHandler {
   private:
    TerminateHandler(const TerminateHandler &) = delete;
    TerminateHandler(TerminateHandler &&)      = delete;
    TerminateHandler &operator=(const TerminateHandler &) = delete;
    TerminateHandler &operator=(TerminateHandler &&) = delete;

   public:
    /**
     * Define singleton behavior using static initializer (cf. http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3242.pdf, Sec. 6.7.4).
     * @return singleton for an instance of this class.
     */
    static TerminateHandler &instance() noexcept {
        static TerminateHandler instance;
        return instance;
    }

    ~TerminateHandler() = default;

   public:
    std::atomic<bool> isTerminated{false};

   private:
    TerminateHandler() noexcept;

#ifndef WIN32
    struct sigaction m_signalHandler {};
#endif
};
} // namespace cluon

#endif
