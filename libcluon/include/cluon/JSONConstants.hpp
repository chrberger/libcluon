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
