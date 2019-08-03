/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_FILTER_HPP
#define CLUON_FILTER_HPP

#include "cluon/cluon.hpp"
#include "cluon/Envelope.hpp"
#include "cluon/stringtoolbox.hpp"

#include <cstdint>
#include <iostream>
#include <string>

inline int32_t cluon_filter(int32_t argc, char **argv) {
    int32_t retCode{0};
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ( ( (0 == commandlineArguments.count("keep")) && (0 == commandlineArguments.count("drop")) )
         || ( (1 == commandlineArguments.count("keep")) && (1 == commandlineArguments.count("drop")) ) ) {
        std::cerr << argv[0] << " filters Envelopes from stdin to stdout." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --keep=<list of messageID/senderStamp pairs to keep> --drop=<list of messageID/senderStamp pairs to drop>" << std::endl;
        std::cerr << "Example: " << argv[0] << " --keep=19/0,25/1" << std::endl;
        std::cerr << "         " << argv[0] << " --drop=19/0,25/1" << std::endl;
        std::cerr << "         --keep and --drop cannot be used simultaneously." << std::endl;
        retCode = 1;
    } else {
        std::map<std::string, bool> mapOfEnvelopesToKeep{};
        {
            std::string tmp{commandlineArguments["keep"]};
            tmp += ",";
            auto entries = stringtoolbox::split(tmp, ',');
            for (auto e : entries) {
                // Use only valid entries.
                if (0 != e.size()) {
                    auto l = stringtoolbox::split(e, '/');
                    std::string toKeep{e};
                    if (0 == l.size()) {
                        toKeep += "/0";
                    }
                    std::cerr << argv[0] << " keeping " << toKeep << std::endl;
                    mapOfEnvelopesToKeep[toKeep] = true;
                }
            }
        }
        std::map<std::string, bool> mapOfEnvelopesToDrop{};
        {
            std::string tmp{commandlineArguments["drop"]};
            tmp += ",";
            auto entries = stringtoolbox::split(tmp, ',');
            for (auto e : entries) {
                // Use only valid entries.
                if (0 != e.size()) {
                    auto l = stringtoolbox::split(e, '/');
                    std::string toDrop{e};
                    if (0 == l.size()) {
                        toDrop += "/0";
                    }
                    std::cerr << argv[0] << " dropping " << toDrop << std::endl;
                    mapOfEnvelopesToDrop[toDrop] = true;
                }
            }
        }

        bool foundData{false};
        do {
            auto retVal = cluon::extractEnvelope(std::cin);
            foundData = retVal.first;
            if (0 < retVal.second.dataType()) {
                std::stringstream sstr;
                sstr << retVal.second.dataType() << "/" << retVal.second.senderStamp();
                std::string str = sstr.str();
                if ( (0 < mapOfEnvelopesToKeep.size()) && mapOfEnvelopesToKeep.count(str)) {
                    str = cluon::serializeEnvelope(std::move(retVal.second));
                    std::cout << str;
                    std::cout.flush();
                }
                if ( (0 < mapOfEnvelopesToDrop.size()) && !mapOfEnvelopesToDrop.count(str)) {
                    str = cluon::serializeEnvelope(std::move(retVal.second));
                    std::cout << str;
                    std::cout.flush();
                }
            }
        } while (std::cin.good() && foundData);
    }
    return retCode;
}

#endif

