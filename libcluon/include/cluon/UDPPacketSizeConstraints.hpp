/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_UDPPACKETSIZECONSTRAINTS_H
#define CLUON_UDPPACKETSIZECONSTRAINTS_H

#include <cstdint>

// clang-format off
namespace cluon {
    enum class UDPPacketSizeConstraints : uint16_t {
        SIZE_IPv4_HEADER    = 20,
        SIZE_UDP_HEADER     = 8,
        MAX_SIZE_UDP_PACKET = 0xFFFF, };
}
// clang-format on

#endif
