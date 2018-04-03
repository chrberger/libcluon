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

#include "cluon/Envelope.hpp"
#include "cluon/FromProtoVisitor.hpp"
#include "cluon/OD4Session.hpp"
#include "cluon/Time.hpp"
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

TEST_CASE("Create OD4 session and transmit data no sample time stamp.") {
    std::atomic<bool> replyReceived{false};
    cluon::data::Envelope reply;
    REQUIRE(0 == reply.dataType());

    cluon::OD4Session od4(79, [&reply, &replyReceived](cluon::data::Envelope &&envelope) {
        reply         = envelope;
        replyReceived = true;
    });
    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp tsRequest;
    tsRequest.seconds(1).microseconds(2);
    od4.send(tsRequest);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!replyReceived);

    REQUIRE(reply.dataType() == cluon::data::TimeStamp::ID());

    cluon::data::TimeStamp tsResponse;
    REQUIRE(0 == tsResponse.seconds());
    REQUIRE(0 == tsResponse.microseconds());

    tsResponse = cluon::extractMessage<cluon::data::TimeStamp>(std::move(reply));
    REQUIRE(1 == tsResponse.seconds());
    REQUIRE(2 == tsResponse.microseconds());
}

TEST_CASE("Create OD4 session and transmit data with sample time stamp.") {
    std::atomic<bool> replyReceived{false};
    cluon::data::Envelope reply;
    REQUIRE(0 == reply.dataType());

    cluon::OD4Session od4(80, [&reply, &replyReceived](cluon::data::Envelope &&envelope) {
        reply         = envelope;
        replyReceived = true;
    });
    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp tsSampleTime;
    tsSampleTime.seconds(10).microseconds(20);

    cluon::data::TimeStamp tsRequest;
    tsRequest.seconds(1).microseconds(2);
    od4.send(tsRequest, tsSampleTime);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!replyReceived);

    REQUIRE(reply.dataType() == cluon::data::TimeStamp::ID());

    cluon::data::TimeStamp tsResponse;
    REQUIRE(0 == tsResponse.seconds());
    REQUIRE(0 == tsResponse.microseconds());

    REQUIRE(10 == reply.sampleTimeStamp().seconds());
    REQUIRE(20 == reply.sampleTimeStamp().microseconds());

    tsResponse = cluon::extractMessage<cluon::data::TimeStamp>(std::move(reply));
    REQUIRE(1 == tsResponse.seconds());
    REQUIRE(2 == tsResponse.microseconds());
}

TEST_CASE("Create OD4 session with unrelated dataTrigger and transmit data with sample time stamp.") {
    std::atomic<bool> replyNotReceived{true};

    cluon::OD4Session od4(81);

    auto dataTrigger = [&replyNotReceived](cluon::data::Envelope &&) {
        replyNotReceived = false;
    };

    bool retVal = od4.dataTrigger(cluon::data::TimeStamp::ID()+2, dataTrigger);
    REQUIRE(retVal);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp tsSampleTime;
    tsSampleTime.seconds(100).microseconds(200);

    cluon::data::TimeStamp tsRequest;
    tsRequest.seconds(3).microseconds(4);
    od4.send(tsRequest, tsSampleTime);

    using namespace std::literals::chrono_literals; // NOLINT
    std::this_thread::sleep_for(1000ms);

    REQUIRE(replyNotReceived);
}

TEST_CASE("Create OD4 session with dataTrigger and transmit data with sample time stamp.") {
    std::atomic<bool> replyReceived{false};
    cluon::data::Envelope reply;
    REQUIRE(0 == reply.dataType());

    cluon::OD4Session od4(82);

    auto dataTrigger = [&reply, &replyReceived](cluon::data::Envelope &&envelope) {
        reply         = envelope;
        replyReceived = true;
    };

    bool retVal = od4.dataTrigger(cluon::data::TimeStamp::ID(), dataTrigger);
    REQUIRE(retVal);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp tsSampleTime;
    tsSampleTime.seconds(100).microseconds(200);

    cluon::data::TimeStamp tsRequest;
    tsRequest.seconds(3).microseconds(4);
    od4.send(tsRequest, tsSampleTime);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!replyReceived);

    REQUIRE(reply.dataType() == cluon::data::TimeStamp::ID());

    cluon::data::TimeStamp tsResponse;
    REQUIRE(0 == tsResponse.seconds());
    REQUIRE(0 == tsResponse.microseconds());

    REQUIRE(100 == reply.sampleTimeStamp().seconds());
    REQUIRE(200 == reply.sampleTimeStamp().microseconds());

    tsResponse = cluon::extractMessage<cluon::data::TimeStamp>(std::move(reply));
    REQUIRE(3 == tsResponse.seconds());
    REQUIRE(4 == tsResponse.microseconds());
}

TEST_CASE("Create OD4 session with catch-all delegate disables with dataTrigger delegate.") {
    std::atomic<bool> replyNotReceivedDataTrigger{true};

    std::atomic<bool> replyReceived{false};
    cluon::data::Envelope reply;
    REQUIRE(0 == reply.dataType());

    cluon::OD4Session od4(83, [&reply, &replyReceived](cluon::data::Envelope &&envelope) {
        reply         = envelope;
        replyReceived = true;
    });

    auto dataTrigger = [&replyNotReceivedDataTrigger](cluon::data::Envelope &&) {
        replyNotReceivedDataTrigger = false;
    };

    bool retVal = od4.dataTrigger(cluon::data::TimeStamp::ID(), dataTrigger);
    REQUIRE(!retVal);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp tsSampleTime;
    tsSampleTime.seconds(101).microseconds(201);

    cluon::data::TimeStamp tsRequest;
    tsRequest.seconds(5).microseconds(6);
    od4.send(tsRequest, tsSampleTime);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!replyReceived);

    REQUIRE(replyNotReceivedDataTrigger);
    REQUIRE(reply.dataType() == cluon::data::TimeStamp::ID());

    cluon::data::TimeStamp tsResponse;
    REQUIRE(0 == tsResponse.seconds());
    REQUIRE(0 == tsResponse.microseconds());

    REQUIRE(101 == reply.sampleTimeStamp().seconds());
    REQUIRE(201 == reply.sampleTimeStamp().microseconds());

    tsResponse = cluon::extractMessage<cluon::data::TimeStamp>(std::move(reply));
    REQUIRE(5 == tsResponse.seconds());
    REQUIRE(6 == tsResponse.microseconds());
}

TEST_CASE("Create OD4 session timeTrigger delegate.") {
    cluon::OD4Session od4(84);

    int32_t counter{0};
    auto timeTrigger = [&counter]() {
        if (counter++ < 1) {
            return true;
        }
        else {
            return false;
        }
    };

    od4.timeTrigger(20.0, timeTrigger);
    REQUIRE(2 == counter);
}

TEST_CASE("Create OD4 session timeTrigger delegate with invalid freq.") {
    cluon::OD4Session od4(85);

    int32_t counter{0};
    auto timeTrigger = [&counter]() {
        if (counter++ < 1) {
            return true;
        }
        else {
            return false;
        }
    };

    cluon::data::TimeStamp before{cluon::time::now()};
    od4.timeTrigger(0, timeTrigger);
    cluon::data::TimeStamp after{cluon::time::now()};
    REQUIRE(2 == counter);
    REQUIRE(2 * 1000 * 1000 <=((after.seconds()*1000*1000 + after.microseconds()) - (before.seconds()*1000*1000 + before.microseconds())));
}

TEST_CASE("Create OD4 session timeTrigger delegate throwing exception cancels timeTrigger.") {
    cluon::OD4Session od4(86);

    int32_t counter{0};
    auto timeTrigger = [&counter]() {
        if (counter++ < 2) {
            return true;
        }
        else {
            throw std::string("Exception");
        }
    };

    od4.timeTrigger(10, timeTrigger);
    REQUIRE(3 == counter);
}

TEST_CASE("Create OD4 session timeTrigger delegate running too slowly results in no waiting.") {
    cluon::OD4Session od4(87);

    int32_t counter{0};
    auto timeTrigger = [&counter]() {
        if (counter++ < 1) {
            std::this_thread::sleep_for(std::chrono::duration<int64_t, std::milli>(200));
            return true;
        }
        else {
            throw false;
        }
    };

    cluon::data::TimeStamp before{cluon::time::now()};
    od4.timeTrigger(10, timeTrigger);
    cluon::data::TimeStamp after{cluon::time::now()};
    REQUIRE(2 == counter);
    REQUIRE(200 * 1000 <=((after.seconds()*1000*1000 + after.microseconds()) - (before.seconds()*1000*1000 + before.microseconds())));
}
