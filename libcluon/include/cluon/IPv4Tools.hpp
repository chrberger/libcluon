/*
 * Copyright (C) 2019  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_IPV4TOOLS_HPP
#define CLUON_IPV4TOOLS_HPP

#include <string>

namespace cluon {

/**
 * @return IPv4-formatted string for the given hostname or the empty string.
 */
std::string getIPv4FromHostname(const std::string &hostname) noexcept;

} // namespace cluon

#endif
