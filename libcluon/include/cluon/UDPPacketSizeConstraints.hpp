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
