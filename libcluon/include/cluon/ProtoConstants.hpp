/*
 * Copyright (C) 2017  Christian Berger
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

#ifndef PROTOCONSTANTS_HPP
#define PROTOCONSTANTS_HPP

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