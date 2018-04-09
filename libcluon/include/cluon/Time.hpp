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

#ifndef TIME_HPP
#define TIME_HPP

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
    ts.seconds(static_cast<int32_t>(tp/static_cast<int64_t>(1000*1000)))
      .microseconds(static_cast<int32_t>(tp%static_cast<int64_t>(1000*1000)));
    return ts;
}

/**
 * @param tp to be converted to microseconds.
 * @return TimeStamp converted to microseconds.
 */
inline int64_t toMicroseconds(const cluon::data::TimeStamp &tp) noexcept {
    return static_cast<int64_t>(tp.seconds())*static_cast<int64_t>(1000*1000) + static_cast<int64_t>(tp.microseconds());
}

/**
 * @param AFTER First time stamp.
 * @param BEFORE Second time stamp.
 * @return Delta (BEFORE - AFTER) between two TimeStamps in microseconds.
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
 * @return TimeStamp of now.
 */
inline cluon::data::TimeStamp now() noexcept {
    return convert(std::chrono::system_clock::now());
}

} // namespace time
} // namespace cluon

#endif
