/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_CLUON_HPP
#define CLUON_CLUON_HPP

// clang-format off
#ifdef WIN32
    #ifdef _WIN64
        #define ssize_t __int64
    #else
        #define ssize_t long
    #endif

    // Disable warning "'<': signed/unsigned mismatch".
    #pragma warning(disable : 4018)
    // Disable warning "Unary minus operator applied to unsigned type, result still unsigned".
    #pragma warning(disable : 4146)
    // Disable warning "Possible loss of precision".
    #pragma warning(disable : 4244)
    // Disable warning "Conversion from 'size_t' to 'type', possible loss of data".
    #pragma warning(disable : 4267)
    // Disable warning "'static_cast': truncation of constant value".
    #pragma warning(disable : 4309)
    // Disable warning "'operator ""s': literal suffix identifiers that do not start with an underscore are reserved".
    #pragma warning(disable : 4455)
    // Disable deprecated API warnings.
    #pragma warning(disable : 4996)

    // Link against ws2_32.lib for networking.
    #pragma comment(lib, "ws2_32.lib")
    // Link against iphlpapi.lib for address resolving.
    #pragma comment(lib, "iphlpapi.lib")

    // Avoid include definitions from Winsock v1.
    #define WIN32_LEAN_AND_MEAN

    // Export symbols.
    #ifdef LIBCLUON_SHARED
        #ifdef LIBCLUON_EXPORTS
            #define LIBCLUON_API __declspec(dllexport)
        #else
            #define LIBCLUON_API __declspec(dllimport)
        #endif
    #else
        // If linking statically:
        #define LIBCLUON_API
    #endif
#else
    // Undefine define for non-Win32 systems:
    #define LIBCLUON_API
#endif
// clang-format on

#include "cluon/PortableEndian.hpp"

#include <map>
#include <string>

namespace cluon {

/**
 * @return Map for command line parameters passed as --key=value into key->values.
 */
std::map<std::string, std::string> getCommandlineArguments(int32_t argc, char **argv) noexcept;

} // namespace cluon

#endif
