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
                auto l = stringtoolbox::split(e, '/');
                std::string toKeep{e};
                if (0 == l.size()) {
                    toKeep += "/0";
                }
                std::cerr << argv[0] << " keeping " << toKeep << std::endl;
                mapOfEnvelopesToKeep[toKeep] = true;
            }
        }
        std::map<std::string, bool> mapOfEnvelopesToDrop{};
        {
            std::string tmp{commandlineArguments["drop"]};
            tmp += ",";
            auto entries = stringtoolbox::split(tmp, ',');
            for (auto e : entries) {
                auto l = stringtoolbox::split(e, '/');
                std::string toDrop{e};
                if (0 == l.size()) {
                    toDrop += "/0";
                }
                std::cerr << argv[0] << " dropping " << toDrop << std::endl;
                mapOfEnvelopesToDrop[toDrop] = true;
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

