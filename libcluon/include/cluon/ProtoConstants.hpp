/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_PROTOCONSTANTS_HPP
#define CLUON_PROTOCONSTANTS_HPP

#include <cstdint>

// clang-format off
namespace cluon {
    enum class ProtoConstants : uint8_t {
        VARINT           = 0,
        EIGHT_BYTES      = 1,
        LENGTH_DELIMITED = 2,
        FOUR_BYTES       = 5, };
}
// clang-format on

#endif
