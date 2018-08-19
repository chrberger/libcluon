/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_MSC_HPP
#define CLUON_MSC_HPP

#include "cluon/MessageParser.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/MetaMessageToCPPTransformator.hpp"
#include "cluon/MetaMessageToProtoTransformator.hpp"

#include "argh/argh.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

inline int32_t cluon_msc(int32_t argc, char **argv) {
    const std::string PROGRAM{argv[0]}; // NOLINT
    argh::parser commandline(argc, argv);

    std::string inputFilename = commandline.pos_args().back();
    if (std::string::npos != inputFilename.find(PROGRAM)) {
        std::cerr << PROGRAM
                  << " transforms a given message specification file in .odvd format into C++." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " [--cpp] [--proto] [--out=<file>] <odvd file>" << std::endl;
        std::cerr << "         " << PROGRAM << " --cpp:   Generate C++14-compliant, self-contained header file." << std::endl;
        std::cerr << "         " << PROGRAM << " --proto: Generate Proto version2-compliant file." << std::endl;
        std::cerr << std::endl;
        std::cerr << "Example: " << PROGRAM << " --cpp --out=/tmp/myOutput.hpp myFile.odvd" << std::endl;
        return 1;
    }

    std::string outputFilename;
    commandline({"--out"}) >> outputFilename;

    const bool generateCPP = commandline[{"--cpp"}];
    const bool generateProto = commandline[{"--proto"}];

    int retVal = 1;
    std::ifstream inputFile(inputFilename, std::ios::in);
    if (inputFile.good()) {
        bool addHeaderForFirstProtoFile = true;
        std::string input(static_cast<std::stringstream const&>(std::stringstream() << inputFile.rdbuf()).str()); // NOLINT

        cluon::MessageParser mp;
        auto result = mp.parse(input);
        retVal = result.second;

        // Delete the content of a potentially existing file.
        if (!outputFilename.empty()) {
            std::ofstream outputFile(outputFilename, std::ios::out | std::ios::trunc);
            outputFile.close();
        }
        for (auto e : result.first) {
            std::string content;
            if (generateCPP) {
                cluon::MetaMessageToCPPTransformator transformation;
                e.accept([&trans = transformation](const cluon::MetaMessage &_mm){ trans.visit(_mm); });
                content = transformation.content();
            }
            if (generateProto) {
                cluon::MetaMessageToProtoTransformator transformation;
                e.accept([&trans = transformation](const cluon::MetaMessage &_mm){ trans.visit(_mm); });
                content = transformation.content(addHeaderForFirstProtoFile);
                addHeaderForFirstProtoFile = false;
            }

            if (!outputFilename.empty()) {
                std::ofstream outputFile(outputFilename, std::ios::out | std::ios::app);
                outputFile << content << std::endl;
                outputFile.close();
            }
            else { // LCOV_EXCL_LINE
                std::cout << content << std::endl; // LCOV_EXCL_LINE
            }
        }
    }
    else {
        std::cerr << "[" << PROGRAM << "] Could not find '" << inputFilename << "'." << std::endl;
    }

    return retVal;
}

#endif
