/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_REC2CSV_HPP
#define CLUON_REC2CSV_HPP

#include "cluon/cluon.hpp"
#include "cluon/GenericMessage.hpp"
#include "cluon/MessageParser.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/Player.hpp"
#include "cluon/ToCSVVisitor.hpp"
#include "cluon/stringtoolbox.hpp"
#include "cluon/cluonDataStructures.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>

inline int32_t cluon_rec2csv(int32_t argc, char **argv) {
    int32_t retCode{0};
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ( (0 == commandlineArguments.count("rec")) || (0 == commandlineArguments.count("odvd")) ) {
        std::cerr << argv[0] << " extracts the content from a given .rec file using a provided .odvd message specification into separate .csv files." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --rec=<Recording from an OD4Session> --odvd=<ODVD Message Specification>" << std::endl;
        std::cerr << "Example: " << argv[0] << " --rec=myRecording.rec --odvd=myMessages.odvd" << std::endl;
        retCode = 1;
    } else {
        // Maps of container-ID & sender-stamp.
        std::map<std::string, std::string> mapOfFilenames;
        std::map<std::string, std::string> mapOfEntries;
        std::map<std::string, size_t> mapOfEntriesSizes;
        std::map<std::string, bool> mapOfFilenamesThatHaveBeenReset;

        cluon::MessageParser mp;
        std::pair<std::vector<cluon::MetaMessage>, cluon::MessageParser::MessageParserErrorCodes> messageParserResult;
        {
            std::ifstream fin(commandlineArguments["odvd"], std::ios::in|std::ios::binary);
            if (fin.good()) {
                std::string input(static_cast<std::stringstream const&>(std::stringstream() << fin.rdbuf()).str()); // NOLINT
                fin.close();
                messageParserResult = mp.parse(input);
                std::clog << "Found " << messageParserResult.first.size() << " messages." << std::endl;
            }
            else {
                std::cerr << argv[0] << ": Message specification '" << commandlineArguments["odvd"] << "' not found." << std::endl;
                return retCode = 1;
            }
        }

        std::fstream fin(commandlineArguments["rec"], std::ios::in|std::ios::binary);
        if (fin.good()) {
            fin.close();

            auto fileWriter = [argv, &mapOfFilenames, &mapOfEntries, &mapOfEntriesSizes, &mapOfFilenamesThatHaveBeenReset](){
              for(auto entries : mapOfFilenames) {
                  std::cerr << argv[0] << " writing '" << entries.second << ".csv'...";
                  // Reset files on first access.
                  std::ios_base::openmode openMode = std::ios::out|std::ios::binary|(mapOfFilenamesThatHaveBeenReset.count(entries.second) == 0 ? std::ios::trunc : std::ios::app);
                  std::fstream fout(entries.second + ".csv", openMode);
                  if (fout.good() && mapOfEntries.count(entries.first)) {
                      const std::string tmp{mapOfEntries[entries.first]};
                      fout.write(tmp.c_str(), static_cast<std::streamsize>(tmp.size()));
                      // Reset memory.
                      mapOfEntries[entries.first] = "";
                      mapOfEntriesSizes[entries.first] = 0;
                  }
                  fout.close();
                  mapOfFilenamesThatHaveBeenReset[entries.second] = true;
                  std::cerr << " done." << std::endl;
              }
            };

            std::map<int32_t, cluon::MetaMessage> scope;
            for (const auto &e : messageParserResult.first) { scope[e.messageIdentifier()] = e; }

            constexpr const bool AUTOREWIND{false};
            constexpr const bool THREADING{false};
            cluon::Player player(commandlineArguments["rec"], AUTOREWIND, THREADING);

            constexpr const size_t TEN_MB{10*1024*1024};
            uint32_t envelopeCounter{0};
            int32_t oldPercentage = -1;
            while (player.hasMoreData()) {
                auto next = player.getNextEnvelopeToBeReplayed();
                if (next.first) {
                    {
                        envelopeCounter++;
                        const int32_t percentage = static_cast<int32_t>((static_cast<float>(envelopeCounter)*100.0f)/static_cast<float>(player.totalNumberOfEnvelopesInRecFile()));
                        if ( (percentage % 5 == 0) && (percentage != oldPercentage) ) {
                            std::cerr << argv[0] << ": Processed " << percentage << "%." << std::endl;
                            oldPercentage = percentage;
                        }
                    }
                    cluon::data::Envelope env{std::move(next.second)};
                    if (scope.count(env.dataType()) > 0) {
                        cluon::FromProtoVisitor protoDecoder;
                        std::stringstream sstr(env.serializedData());
                        protoDecoder.decodeFrom(sstr);

                        cluon::MetaMessage m = scope[env.dataType()];
                        cluon::GenericMessage gm;
                        gm.createFrom(m, messageParserResult.first);
                        gm.accept(protoDecoder);

                        std::stringstream sstrKey;
                        sstrKey << env.dataType() << "/" << env.senderStamp();
                        const std::string KEY = sstrKey.str();

                        std::stringstream sstrFilename;
                        sstrFilename << m.messageName() << "-" << env.senderStamp();
                        const std::string __FILENAME = sstrFilename.str();
                        mapOfFilenames[KEY] = __FILENAME;

                        if (mapOfEntries.count(KEY) > 0) {
                            // Extract timestamps.
                            std::string timeStamps;
                            {
                                cluon::ToCSVVisitor csv(';', false, { {1,false}, {2,false}, {3,true}, {4,true}, {5,true}, {6,false} });
                                env.accept(csv);
                                timeStamps = csv.csv();
                            }

                            cluon::ToCSVVisitor csv(';', false);
                            gm.accept(csv);

                            std::string entry{stringtoolbox::split(timeStamps, '\n')[0] + csv.csv()};
                            mapOfEntries[KEY] += entry;
                            mapOfEntriesSizes[KEY] += entry.size();
                       }
                        else {
                            // Extract timestamps.
                            std::vector<std::string> timeStampsWithHeader;
                            {
                                // Skip senderStamp (as it is in file name) and serializedData.
                                cluon::ToCSVVisitor csv(';', true, { {1,false}, {2,false}, {3,true}, {4,true}, {5,true}, {6,false} });
                                env.accept(csv);
                                timeStampsWithHeader = stringtoolbox::split(csv.csv(), '\n');
                            }

                            cluon::ToCSVVisitor csv(';', true);
                            gm.accept(csv);

                            std::vector<std::string> valuesWithHeader = stringtoolbox::split(csv.csv(), '\n');
                            std::string entry{timeStampsWithHeader.at(0) + valuesWithHeader.at(0) + '\n' + timeStampsWithHeader.at(1) + valuesWithHeader.at(1) + '\n'};
                            mapOfEntries[KEY] += entry;
                            mapOfEntriesSizes[KEY] += entry.size();
                        }

                        // Keep track of buffer sizes.
                        if (mapOfEntriesSizes[KEY] > TEN_MB) {
                            std::cerr << argv[0] << ": Buffer for '" << KEY << "' has consumed " << mapOfEntriesSizes[KEY] << "/" << TEN_MB << " bytes; dumping data to disk."<< std::endl; // LCOV_EXCL_LINE
                            fileWriter(); // LCOV_EXCL_LINE
                        }
                    }
                }
            }
            // Clear buffer at the end.
            fileWriter();
        }
        else {
            std::cerr << argv[0] << ": Recording '" << commandlineArguments["rec"] << "' not found." << std::endl;
            retCode = 1;
        }
    }
    return retCode;
}

#endif
