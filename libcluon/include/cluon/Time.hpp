/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_TIME_HPP
#define CLUON_TIME_HPP

#include "cluon/cluonDataStructures.hpp"

#include <chrono>

namespace cluon {
namespace time {

/**
 * @param tp to be converted to TimeStamp.
 * @return TimeStamp converted from microseconds.
 */
inline cluon::data::TimeStamp fromMicroseconds(int64_t tp) noexcept {
    cluon::data::TimeStamp ts;
    ts.seconds(static_cast<int32_t>(tp / static_cast<int64_t>(1000 * 1000))).microseconds(static_cast<int32_t>(tp % static_cast<int64_t>(1000 * 1000)));
    return ts;
}

/**
 * @param tp to be converted to microseconds.
 * @return TimeStamp converted to microseconds.
 */
inline int64_t toMicroseconds(const cluon::data::TimeStamp &tp) noexcept {
    return static_cast<int64_t>(tp.seconds()) * static_cast<int64_t>(1000 * 1000) + static_cast<int64_t>(tp.microseconds());
}

/**
 * @param AFTER First time stamp.
 * @param BEFORE Second time stamp.
 * @return Delta (AFTER - BEFORE) between two TimeStamps in microseconds.
 */
inline int64_t deltaInMicroseconds(const cluon::data::TimeStamp &AFTER, const cluon::data::TimeStamp &BEFORE) noexcept {
    return toMicroseconds(AFTER) - toMicroseconds(BEFORE);
}

/**
 * @param tp to be converted to microseconds.
 * @return TimeStamp of converted chrono::time_point.
 */
inline cluon::data::TimeStamp convert(const std::chrono::system_clock::time_point &tp) noexcept {
    cluon::data::TimeStamp timeStamp;

    // Transform chrono time representation to same behavior as gettimeofday.
    typedef std::chrono::duration<int32_t> seconds_type;
    typedef std::chrono::duration<int64_t, std::micro> microseconds_type;

    auto duration                = tp.time_since_epoch();
    seconds_type s               = std::chrono::duration_cast<seconds_type>(duration);
    microseconds_type us         = std::chrono::duration_cast<microseconds_type>(duration);
    microseconds_type partial_us = us - std::chrono::duration_cast<microseconds_type>(s);

    timeStamp.seconds(s.count()).microseconds(static_cast<int32_t>(partial_us.count()));

    return timeStamp;
}

/**
 * @return TimeStamp of now from std::chrono::system_clock.
 */
inline cluon::data::TimeStamp now() noexcept {
    return convert(std::chrono::system_clock::now());
}

} // namespace time
} // namespace cluon

#endif
