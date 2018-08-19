/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_MSGPACKCONSTANTS_HPP
#define CLUON_MSGPACKCONSTANTS_HPP

#include <cstdint>

// clang-format off
namespace cluon {
    enum class MsgPackConstants : uint16_t {
        IS_FALSE        = 0xC2,
        IS_TRUE         = 0xC3,
        FLOAT           = 0xCA,
        DOUBLE          = 0xCB,
        UINT8           = 0xCC,
        UINT16          = 0xCD,
        UINT32          = 0xCE,
        UINT64          = 0xCF,
        NEGFIXINT       = 0xE0,
        INT8            = 0xD0,
        INT16           = 0xD1,
        INT32           = 0xD2,
        INT64           = 0xD3,
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
        UINT_FORMAT     = 0xFF02,
        INT_FORMAT      = 0xFF03,
        FLOAT_FORMAT    = 0xFF04,
        STR_FORMAT      = 0xFF05,
        MAP_FORMAT      = 0xFF06, // Indicating also nested types.
   };
}
// clang-format on

#endif
