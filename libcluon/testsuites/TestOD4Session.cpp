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

#include "catch.hpp"

#include "cluon/OD4Session.hpp"
#include "cluon/FromProtoVisitor.hpp"
#include "cluon/cluonDataStructures.hpp"

#include <atomic>
#include <chrono>
#include <sstream>
#include <thread>

TEST_CASE("Create OD4 session without lambda.") {
    cluon::OD4Session od4(78);
    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp tsRequest;
    tsRequest.seconds(1).microseconds(2);
    od4.send(tsRequest);
}

TEST_CASE("Create OD4 session and transmit data.") {
    std::atomic<bool> replyReceived{false};
    cluon::data::Envelope reply;
    REQUIRE(0 == reply.dataType());

    cluon::OD4Session od4(79, [&reply, &replyReceived](cluon::data::Envelope &&envelope){ reply = envelope; replyReceived = true; });
    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp tsRequest;
    tsRequest.seconds(1).microseconds(2);
    od4.send(tsRequest);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!replyReceived);

    REQUIRE(reply.dataType() == cluon::data::TimeStamp::ID());
    std::stringstream sstr{reply.serializedData()};
    cluon::FromProtoVisitor decoder;
    decoder.decodeFrom(sstr);

    cluon::data::TimeStamp tsResponse;
    REQUIRE(0 == tsResponse.seconds());
    REQUIRE(0 == tsResponse.microseconds());

    tsResponse.accept(decoder);
    REQUIRE(1 == tsResponse.seconds());
    REQUIRE(2 == tsResponse.microseconds());
}

