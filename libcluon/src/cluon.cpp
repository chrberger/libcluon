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

#include "argh/argh.h"

#include "cluon/cluon.hpp"

namespace cluon {

std::map<std::string, std::string> getCommandlineArguments(int32_t argc, char **argv) noexcept {
    argh::parser commandline{argc, argv};
    std::map<std::string, std::string> retVal;

    for (auto &positionalArgument : commandline.pos_args()) { retVal[positionalArgument] = ""; }

    for (auto &flag : commandline.flags()) { retVal[flag] = "1"; }

    for (auto &parameter : commandline.params()) { retVal[parameter.first] = parameter.second; }

    return retVal;
}

} // namespace cluon
