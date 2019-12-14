/*
 * Copyright (C) 2017-2019  Christian Berger
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
#include <limits>
#include <string>

inline int32_t cluon_filter(int32_t argc, char **argv) {
    int32_t retCode{0};
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ( ( (0 == commandlineArguments.count("keep")) && (0 == commandlineArguments.count("drop")) )
         || ( (1 == commandlineArguments.count("keep")) && (1 == commandlineArguments.count("drop")) ) ) {
        std::cerr << argv[0] << " filters Envelopes from stdin to stdout." << std::endl;
        std::cerr << "NOTE! To use the --start/--stop filters, the Envelopes must be chronologically sorted when using --exit." << std::endl;
        std::cerr << "If you are in doubt, simply use cluon-replay and replay to stdout to feed this filter as cluon-replay is sorting by sample timestamp." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --keep=<list of messageID/senderStamp pairs to keep> --drop=<list of messageID/senderStamp pairs to drop> [--skip=<number of Envelopes to skip>] [--start=<keep Envelopes after this timepoint in Epoch seconds>] [--end=<keep Envelopes until this timepoint in Epoch seconds> [--exit]]" << std::endl;
        std::cerr << "Example: " << argv[0] << " --keep=19/0,25/1" << std::endl;
        std::cerr << "         " << argv[0] << " --drop=19/0,25/1" << std::endl;
        std::cerr << "         " << argv[0] << " --skip=300" << std::endl;
        std::cerr << "         " << argv[0] << " --start=1569916731" << std::endl;
        std::cerr << "         " << argv[0] << " --end=1569917000" << std::endl;
        std::cerr << "         " << argv[0] << " --end=+1000  end after 1000s" << std::endl;
        std::cerr << "         " << argv[0] << " --end=1569917000 --exit  exit after first Envelope encountered after end time point" << std::endl;
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

        const uint32_t SKIP{(commandlineArguments.count("skip") != 0) ? static_cast<uint32_t>(std::stoi(commandlineArguments["skip"])) : 0};
        const int32_t START{(commandlineArguments.count("start") != 0) ? static_cast<int32_t>(std::stoi(commandlineArguments["start"])) : 0};
        int32_t END{(commandlineArguments.count("end") != 0) ? static_cast<int32_t>(std::stoi(commandlineArguments["end"])) : (std::numeric_limits<int32_t>::max)()};
        bool isRelativeEnd{false};
        if (commandlineArguments.count("end") != 0) {
            std::string END_TMP = commandlineArguments["end"];
            isRelativeEnd = (END_TMP.at(0) == '+');
        }
        const bool EXIT{commandlineArguments.count("exit") != 0};

        bool foundData{false};
        uint32_t counter{0};
        bool endInitialized{false};
        do {
            auto retVal = cluon::extractEnvelope(std::cin);
            foundData = retVal.first;
            if ( (0 < retVal.second.dataType()) && (retVal.second.dataType() != cluon::data::PlayerStatus::ID()) ) {
                counter++;
                if (counter > SKIP) {
                    cluon::data::TimeStamp sampleTimeStamp = retVal.second.sampleTimeStamp();
                    if (isRelativeEnd && !endInitialized) {
                        endInitialized = true;
                        END += (START > 0 ? START : sampleTimeStamp.seconds());
                        std::cerr << "Keeping Envelopes in the interval (" << START << ", " << END << ")." << std::endl;
                    }
                    if ( (sampleTimeStamp.seconds() > START) && (sampleTimeStamp.seconds() < END) ) {
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
                    if ( !(sampleTimeStamp.seconds() < END) && EXIT ) {
                        exit(0);
                    }
                }
            }
        } while (std::cin.good() && foundData);
    }
    return retCode;
}

#endif

