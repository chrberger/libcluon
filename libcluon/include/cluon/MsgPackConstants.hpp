/*
 * Copyright (C) 2017-2018  Christian Berger
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

#ifndef MSGPACKCONSTANTS_HPP
#define MSGPACKCONSTANTS_HPP

#include <cstdint>

// clang-format off
namespace cluon {
    enum class MsgPackConstants : uint16_t {
        IS_FALSE        = 0xC2,
        IS_TRUE         = 0xC3,
        FIXSTR          = 0xA0,
        FIXSTR_END      = 0xBF,
        STR8            = 0xD9,
        STR16           = 0xDA,
        STR32           = 0xDB,
        FIXMAP          = 0x80,
        FIXMAP_END      = 0x8F,
        MAP16           = 0xDE,
        MAP32           = 0xDF,
        UNKNOWN_FORMAT  = 0xFF00,
        BOOL_FORMAT     = 0xFF01,
        INT_FORMAT      = 0xFF02,
        FLOAT_FORMAT    = 0xFF03,
        STR_FORMAT      = 0xFF04,
        MAP_FORMAT      = 0xFF05, // Indicating also nested types.
   };
}
// clang-format on

#endif
