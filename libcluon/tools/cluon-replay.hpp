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

#ifndef CLUON_REPLAY_HPP
#define CLUON_REPLAY_HPP

#include "cluon/cluon.hpp"
#include "cluon/Envelope.hpp"
#include "cluon/OD4Session.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/Player.hpp"
#include "cluon/cluonDataStructures.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

inline int32_t cluon_replay(int32_t argc, char **argv) {
    int32_t retCode{0};
    const std::string PROGRAM{argv[0]}; // NOLINT
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (1 == argc) {
        std::cerr << PROGRAM << " replays a .rec file into an OpenDaVINCI session or to stdout; if playing back to an OD4Session using parameter --cid, you can specify the optional parameter --stdout to also playback to stdout; --keeprunning keeps " << PROGRAM << " open at the end of a recording file." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " [--cid=<OpenDaVINCI session> [--stdout] [--keeprunning]] recording.rec" << std::endl;
        std::cerr << "Example: " << PROGRAM << " --cid=111 file.rec" << std::endl;
        std::cerr << "         " << PROGRAM << " --cid=111 --stdout file.rec" << std::endl;
        std::cerr << "         " << PROGRAM << " file.rec" << std::endl;
        retCode = 1;
    }
    else {
        const bool playBackToStdout = ( (0 != commandlineArguments.count("stdout")) || (0 == commandlineArguments.count("cid")) );
        const bool keepRunning = (0 != commandlineArguments.count("keeprunning"));

        std::string recFile;
        for (auto e : commandlineArguments) {
            if (recFile.empty() && e.second.empty() && e.first != PROGRAM) {
                recFile = e.first;
                break;
            }
        }

        std::fstream fin(recFile, std::ios::in|std::ios::binary);
        if (fin.good()) {
            std::atomic<bool> playCommandUpdate{false};
            std::mutex playerCommandMutex;
            cluon::data::PlayerCommand playerCommand;

            // Create an OD4Session to relay the.
            std::unique_ptr<cluon::OD4Session> od4;
            if (0 != commandlineArguments.count("cid")) {
                // Interface to a running OpenDaVINCI session and listening for PlayerCommands.
                od4 = std::make_unique<cluon::OD4Session>(static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))); // LCOV_EXCL_LINE
                if (od4) {
                    od4->dataTrigger(cluon::data::PlayerCommand::ID(), [&playCommandUpdate, &playerCommandMutex, &playerCommand](cluon::data::Envelope &&env){
                        cluon::data::PlayerCommand pc = cluon::extractMessage<cluon::data::PlayerCommand>(std::move(env));
                        {
                            std::lock_guard<std::mutex> lck(playerCommandMutex);
                            playerCommand = pc;
                        }
                        playCommandUpdate = true;
                    });
                }
            }

            // Listen for PlayerStatus updates.
            std::atomic<bool> playerStatusUpdate{false};
            std::mutex playerStatusMutex;
            cluon::data::PlayerStatus playerStatus;
            {
                std::string s;
                playerStatus.state(1); // Report: "loading file"
                {
                    std::lock_guard<std::mutex> lck(playerStatusMutex);

                    cluon::ToProtoVisitor protoEncoder;
                    playerStatus.accept(protoEncoder);
                    s = protoEncoder.encodedData();
                }
                cluon::data::Envelope env;
                env.dataType(playerStatus.ID())
                   .sent(cluon::time::now())
                   .sampleTimeStamp(cluon::time::now())
                   .serializedData(s);

                if (od4 && od4->isRunning()) {
                    od4->send(std::move(env));
                }
                else {
                    std::cout << cluon::serializeEnvelope(std::move(env));
                    std::cout.flush();
                }
            }
            constexpr bool AUTOREWIND{false};
            constexpr bool THREADING{true};
            cluon::Player player(recFile, AUTOREWIND, THREADING);
            player.setPlayerListener([&playerStatusUpdate, &playerStatusMutex, &playerStatus](cluon::data::PlayerStatus &&ps){
                {
                    std::lock_guard<std::mutex> lck(playerStatusMutex);
                    playerStatus = ps;
                }
                playerStatusUpdate = true;
            });

            {
                std::string s;
                playerStatus.numberOfEntries(player.totalNumberOfEnvelopesInRecFile());
                playerStatus.state(2); // playback file
                {
                    std::lock_guard<std::mutex> lck(playerStatusMutex);

                    cluon::ToProtoVisitor protoEncoder;
                    playerStatus.accept(protoEncoder);
                    s = protoEncoder.encodedData();
                }
                cluon::data::Envelope env;
                env.dataType(playerStatus.ID())
                   .sent(cluon::time::now())
                   .sampleTimeStamp(cluon::time::now())
                   .serializedData(s);

                if (od4 && od4->isRunning()) {
                    od4->send(std::move(env));
                }
                else {
                    std::cout << cluon::serializeEnvelope(std::move(env));
                    std::cout.flush();
                }
            }

            bool play = true;
            bool step = false;
            while ( (player.hasMoreData() || keepRunning) ) {
                // Stop execution in case of a running OD4Session.
                if (od4 && !od4->isRunning()) {
                    break;
                }
                // If we are at the end of the playback file, simply wait a little to avoid excessive system load.
                if (!player.hasMoreData() && keepRunning) {
                    std::this_thread::sleep_for(std::chrono::duration<int32_t, std::milli>(200)); // LCOV_EXCL_LINE
                }
                // Check for broadcasting status updates.
                if (playerStatusUpdate) {
                    std::string s;
                    {
                        std::lock_guard<std::mutex> lck(playerStatusMutex);

                        cluon::ToProtoVisitor protoEncoder;
                        playerStatus.accept(protoEncoder);
                        s = protoEncoder.encodedData();
                    }
                    cluon::data::Envelope env;
                    env.dataType(playerStatus.ID())
                       .sent(cluon::time::now())
                       .sampleTimeStamp(cluon::time::now())
                       .serializedData(s);

                    if (od4 && od4->isRunning()) {
                        cluon::data::Envelope e = env;
                        od4->send(std::move(e));
                    }
                    if (playBackToStdout) {
                        cluon::data::Envelope e = env;
                        std::cout << cluon::serializeEnvelope(std::move(e));
                        std::cout.flush();
                    }
                    playerStatusUpdate = false;
                }
                // Check for remotely controlling the player.
                if (playCommandUpdate) {
                    std::lock_guard<std::mutex> lck(playerCommandMutex);
                    if ( (playerCommand.command() == 1) || (playerCommand.command() == 2) ) {
                        play = !(2 == playerCommand.command()); // LCOV_EXCL_LINE
                        std::clog << PROGRAM << ": Change state: " << +playerCommand.command() << ", play = " << play << std::endl;
                    }

                    if (3 == playerCommand.command()) {
                        std::clog << PROGRAM << ": Change state: " << +playerCommand.command() << ", seekTo: " << playerCommand.seekTo() << std::endl;
                        player.seekTo(playerCommand.seekTo());
                    }

                    if (4 == playerCommand.command()) {
                        play = false;
                        step = true;
                        std::clog << PROGRAM << ": Change state: " << +playerCommand.command() << ", play = " << play << std::endl;
                    }

                    playCommandUpdate = false;
                }
                // If playback is desired, relay the Envelope to the OD4Session.
                if (play || step) {
                    auto next = player.getNextEnvelopeToBeReplayed();
                    if (next.first) {
                        if (od4 && od4->isRunning()) {
                            cluon::data::Envelope e = next.second;
                            od4->send(std::move(e));
                        }
                        if (playBackToStdout) {
                            cluon::data::Envelope e = next.second;
                            std::cout << cluon::serializeEnvelope(std::move(e));
                            std::cout.flush();
                        }
                        std::this_thread::sleep_for(std::chrono::duration<int32_t, std::micro>(player.delay()));
                    }
                }
                else {
                    std::this_thread::sleep_for(std::chrono::duration<int32_t, std::milli>(100)); // LCOV_EXCL_LINE
                } // LCOV_EXCL_LINE

                // Reset step.
                step = false;
            }
            retCode = 0;
        }
        else {
            std::cerr << PROGRAM << ": file '" << recFile << "' not found." << std::endl;
            retCode = 1;
        }
    }
    return retCode;
}

#endif

