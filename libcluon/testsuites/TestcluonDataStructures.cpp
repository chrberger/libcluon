/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "catch.hpp"

#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"

TEST_CASE("Test cluon::data::TimeStamp.") {
    cluon::data::TimeStamp ts;
    REQUIRE("TimeStamp" == cluon::data::TimeStamp::ShortName());
    REQUIRE("cluon.data.TimeStamp" == cluon::data::TimeStamp::LongName());
    REQUIRE(0 == ts.seconds());
    REQUIRE(0 == ts.microseconds());
    REQUIRE(12 == cluon::data::TimeStamp::ID());
}

TEST_CASE("Test cluon::data::Envelope.") {
    cluon::data::Envelope env;
    REQUIRE("Envelope" == cluon::data::Envelope::ShortName());
    REQUIRE("cluon.data.Envelope" == cluon::data::Envelope::LongName());

    REQUIRE(0 == env.dataType());
    REQUIRE(env.serializedData().empty());
    REQUIRE(0 == env.sent().seconds());
    REQUIRE(0 == env.sent().microseconds());
    REQUIRE(0 == env.received().seconds());
    REQUIRE(0 == env.received().microseconds());
    REQUIRE(0 == env.sampleTimeStamp().seconds());
    REQUIRE(0 == env.sampleTimeStamp().microseconds());
}

TEST_CASE("Test cluon::data::PlayerCommand.") {
    cluon::data::PlayerCommand pc;
    REQUIRE("PlayerCommand" == cluon::data::PlayerCommand::ShortName());
    REQUIRE("cluon.data.PlayerCommand" == cluon::data::PlayerCommand::LongName());

    REQUIRE(0 == pc.command());
    REQUIRE(0.0f == Approx(pc.seekTo()));
}

TEST_CASE("Test cluon::data::PlayerStatus.") {
    cluon::data::PlayerStatus ps;
    REQUIRE("PlayerStatus" == cluon::data::PlayerStatus::ShortName());
    REQUIRE("cluon.data.PlayerStatus" == cluon::data::PlayerStatus::LongName());

    REQUIRE(0 == ps.state());
    REQUIRE(0 == ps.numberOfEntries());
    REQUIRE(0 == ps.currentEntryForPlayback());
}

