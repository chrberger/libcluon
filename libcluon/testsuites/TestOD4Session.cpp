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

#include <iostream>

#include <atomic>
#include <chrono>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

TEST_CASE("Create OD4 session without lambda.") {
    cluon::OD4Session od4(78);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp tsRequest;
    tsRequest.seconds(1).microseconds(2);
    od4.send(tsRequest);

    // Allow for delivery of data.
    std::this_thread::sleep_for(1s);
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

    auto dataTrigger = [&replyNotReceived](cluon::data::Envelope &&) { // LCOV_EXCL_LINE
        replyNotReceived= false;                                          // LCOV_EXCL_LINE
    };                                                                 // LCOV_EXCL_LINE

    bool retVal = od4.dataTrigger(cluon::data::TimeStamp::ID() + 2, dataTrigger);
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

    retVal = od4.dataTrigger(cluon::data::TimeStamp::ID(), nullptr);
    REQUIRE(retVal);
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

    auto dataTrigger = [&replyNotReceivedDataTrigger](cluon::data::Envelope &&) { // LCOV_EXCL_LINE
        replyNotReceivedDataTrigger= false;                                                     // LCOV_EXCL_LINE
    };                                                                            // LCOV_EXCL_LINE

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
        } else {
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
        } else {
            return false;
        }
    };

    cluon::data::TimeStamp before{cluon::time::now()};
    od4.timeTrigger(0, timeTrigger);
    cluon::data::TimeStamp after{cluon::time::now()};
    REQUIRE(2 == counter);
    REQUIRE(2 * 1000 * 1000 <= cluon::time::deltaInMicroseconds(after, before));
}

TEST_CASE("Create OD4 session timeTrigger delegate throwing exception cancels timeTrigger.") {
    cluon::OD4Session od4(86);

    int32_t counter{0};
    auto timeTrigger = [&counter]() {
        if (counter++ < 2) {
            return true;
        } else {
#ifdef WIN32
            return false;
#else
            throw std::string("Exception");
#endif
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
        } else {
            return false;
        }
    };

    cluon::data::TimeStamp before{cluon::time::now()};
    od4.timeTrigger(10, timeTrigger);
    cluon::data::TimeStamp after{cluon::time::now()};
    REQUIRE(2 == counter);
    REQUIRE(200 * 1000 <= cluon::time::deltaInMicroseconds(after, before));
}

TEST_CASE("Create OD4 session with dataTrigger and transmission storm.") {
    std::mutex receivingMutex;
    std::vector<cluon::data::Envelope> receiving;

    cluon::OD4Session od4(88);

    auto dataTrigger = [&receivingMutex, &receiving](cluon::data::Envelope &&envelope) {
        std::lock_guard<std::mutex> lck(receivingMutex);
        receiving.push_back(envelope);
    };

    bool retVal = od4.dataTrigger(cluon::data::TimeStamp::ID(), dataTrigger);
    REQUIRE(retVal);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp before{cluon::time::now()};
    constexpr int32_t MAX_ENVELOPES{30* 1000};
    for(int32_t i{0}; i < MAX_ENVELOPES; i++) {
        cluon::data::TimeStamp tsSampleTime;
        tsSampleTime.seconds(0).microseconds(i);

        od4.send(tsSampleTime);
    }
    cluon::data::TimeStamp after{cluon::time::now()};

    // Wait for processing the sent data.
    std::this_thread::sleep_for(1s);

    std::cout << "Sent " << MAX_ENVELOPES << " (took " << cluon::time::deltaInMicroseconds(after, before)/1000 << " ms). Received " << receiving.size() << " envelopes." << std::endl;

    REQUIRE(receiving.size() > .9f*MAX_ENVELOPES); // At least 90% of the packets must be processed.
}

TEST_CASE("Create OD4 session with dataTrigger and transmission storm from 5 threads.") {
    std::mutex receivingMutex;
    std::vector<cluon::data::Envelope> receiving;

    cluon::OD4Session od4(89);

    auto dataTrigger = [&receivingMutex, &receiving](cluon::data::Envelope &&envelope) {
        std::lock_guard<std::mutex> lck(receivingMutex);
        receiving.push_back(envelope);
    };

    bool retVal = od4.dataTrigger(cluon::data::TimeStamp::ID(), dataTrigger);
    REQUIRE(retVal);

    using namespace std::literals::chrono_literals; // NOLINT
    do { std::this_thread::sleep_for(1ms); } while (!od4.isRunning());

    REQUIRE(od4.isRunning());

    cluon::data::TimeStamp before{cluon::time::now()};

    std::thread sender1([&od4](){
        constexpr int32_t MAX_ENVELOPES{10* 1000};
        for(int32_t i{0}; i < MAX_ENVELOPES; i++) {
            cluon::data::TimeStamp tsSampleTime;
            tsSampleTime.seconds(0).microseconds(i);

            od4.send(tsSampleTime);
        }
    });
    std::thread sender2([&od4](){
        constexpr int32_t MAX_ENVELOPES{10* 1000};
        for(int32_t i{0}; i < MAX_ENVELOPES; i++) {
            cluon::data::TimeStamp tsSampleTime;
            tsSampleTime.seconds(0).microseconds(i);

            od4.send(tsSampleTime);
        }
    });
    std::thread sender3([&od4](){
        constexpr int32_t MAX_ENVELOPES{10* 1000};
        for(int32_t i{0}; i < MAX_ENVELOPES; i++) {
            cluon::data::TimeStamp tsSampleTime;
            tsSampleTime.seconds(0).microseconds(i);

            od4.send(tsSampleTime);
        }
    });

    sender1.join();
    sender2.join();
    sender3.join();
    cluon::data::TimeStamp after{cluon::time::now()};

    // Wait for processing the sent data.
    std::this_thread::sleep_for(1s);

    constexpr int32_t MAX_ENVELOPES{10* 1000};

    std::cout << "Sending of 3 times " << MAX_ENVELOPES << " in parallel took " << cluon::time::deltaInMicroseconds(after, before)/1000 << " ms. Received " << receiving.size() << " envelopes." << std::endl;

    REQUIRE(receiving.size() > .9f*3*MAX_ENVELOPES); // At least 90% of the packets must be processed.
}

