/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_JSONCONSTANTS_HPP
#define CLUON_JSONCONSTANTS_HPP

#include <cstdint>

// clang-format off
namespace cluon {
    enum class JSONConstants : uint16_t {
        IS_FALSE    = 0,
        IS_TRUE     = 1,
        NUMBER      = 2,
        STRING      = 3,
        OBJECT      = 4, // Indicating nested types.
        UNDEFINED   = 99,
   };
}
// clang-format on

#endif
