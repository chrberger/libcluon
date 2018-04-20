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

#include "cluon/SharedMemory.hpp"

#include <chrono>
#include <thread>

TEST_CASE("Trying to open SharedMemory with empty name.") {
    cluon::SharedMemory sm1{""};
    REQUIRE(!sm1.valid());
    REQUIRE(0 == sm1.size());
    REQUIRE(nullptr == sm1.data());
    REQUIRE(sm1.name().empty());
}

TEST_CASE("Trying to open SharedMemory with name without leading /.") {
    cluon::SharedMemory sm1{"ABC"};
    REQUIRE(!sm1.valid());
    REQUIRE(0 == sm1.size());
    REQUIRE(nullptr == sm1.data());
    REQUIRE("/ABC" == sm1.name());
}

TEST_CASE("Trying to open SharedMemory with name without leading / and too long name > 255.") {
    const std::string NAME{"Vlrel3r6cZeWaRsWgvCWfAHtpPKX56fSgNYNM5bMjEcBnuiMOG3g4YJ4Y9KbPcNyes45xPI9jD5FjxEB1GR9WqaWmyqdH6po1O6is2aDecMe8GGlwqkVJtWH5YwlCYgoJ1EiQhqIUVfzp56IY00J6lXJS0uVJrpcMIZuiCsTGTQDG0vPC2EkdbMxe9BPV6a8BnMMumnGKYcqFxiCGrv1SVtLw40zLXTuelQQHiPCFANYlISyhRPt456PMNm7AQJUMHA5"};
    const std::string NAME_254 = "/" + NAME.substr(0, 253);
    cluon::SharedMemory sm1{NAME};
    REQUIRE(!sm1.valid());
    REQUIRE(nullptr == sm1.data());
    REQUIRE(NAME_254 == sm1.name());
}

TEST_CASE("Trying to create SharedMemory with correct name.") {
    cluon::SharedMemory sm1{"/DEF", 4};
    REQUIRE(sm1.valid());
    REQUIRE(4 == sm1.size());
    REQUIRE(nullptr != sm1.data());
    REQUIRE("/DEF" == sm1.name());
    sm1.lock();
    uint32_t *data = reinterpret_cast<uint32_t*>(sm1.data());
    *data = 12345;
    sm1.unlock();

    sm1.lock();
    uint32_t *data2 = reinterpret_cast<uint32_t*>(sm1.data());
    uint32_t tmp = *data2;
    sm1.unlock();
    REQUIRE(12345 == tmp);
}

TEST_CASE("Trying to create SharedMemory with correct name and separate thread to produce data for shared memory.") {
    cluon::SharedMemory sm1{"/GHI", 4};
    REQUIRE(sm1.valid());
    REQUIRE(4 == sm1.size());
    REQUIRE(nullptr != sm1.data());
    REQUIRE("/GHI" == sm1.name());
    sm1.lock();
    uint32_t *data = reinterpret_cast<uint32_t*>(sm1.data());
    REQUIRE(0 == *data);
    sm1.unlock();

    // Spawning thread to attach and change data.
    std::thread producer([](){
        cluon::SharedMemory inner_sm1{"/GHI"};
        REQUIRE(inner_sm1.valid());
        REQUIRE(nullptr != inner_sm1.data());
        REQUIRE("/GHI" == inner_sm1.name());
        inner_sm1.lock();
        uint32_t *inner_data = reinterpret_cast<uint32_t*>(inner_sm1.data());
        REQUIRE(0 == *inner_data);
        *inner_data = 54321;
        REQUIRE(54321 == *inner_data);
        inner_sm1.unlock();
    });

    uint32_t tmp{0};
    do {
        sm1.lock();
        tmp = *(reinterpret_cast<uint32_t*>(sm1.data()));
        sm1.unlock();

        using namespace std::literals::chrono_literals; // NOLINT
        std::this_thread::sleep_for(10ms);
    } while (0 == tmp);

    producer.join();

    REQUIRE(54321 == tmp);
}

TEST_CASE("Trying to create SharedMemory with correct name and separate thread to produce data for shared memory with condition variable for synchronization.") {
    cluon::SharedMemory sm1{"/JKL", 4};
    REQUIRE(sm1.valid());
    REQUIRE(4 == sm1.size());
    REQUIRE(nullptr != sm1.data());
    REQUIRE("/JKL" == sm1.name());
    sm1.lock();
    uint32_t *data = reinterpret_cast<uint32_t*>(sm1.data());
    REQUIRE(0 == *data);
    sm1.unlock();

    // Spawning thread to attach and change data.
    std::thread producer([](){
        cluon::SharedMemory inner_sm1{"/JKL"};
        REQUIRE(inner_sm1.valid());
        REQUIRE(nullptr != inner_sm1.data());
        REQUIRE("/JKL" == inner_sm1.name());
        inner_sm1.lock();
        uint32_t *inner_data = reinterpret_cast<uint32_t*>(inner_sm1.data());
        REQUIRE(0 == *inner_data);
        *inner_data = 23456;
        REQUIRE(23456 == *inner_data);
        inner_sm1.unlock();

        inner_sm1.notifyAll();
    });

    sm1.wait();
    producer.join();

    sm1.lock();
    uint32_t tmp = *(reinterpret_cast<uint32_t*>(sm1.data()));
    sm1.unlock();

    REQUIRE(23456 == tmp);
}

TEST_CASE("Trying to create SharedMemory with correct name and two separate threads to produce data for shared memory with condition variable for synchronization.") {
    cluon::SharedMemory sm1{"/MNO", 4};
    REQUIRE(sm1.valid());
    REQUIRE(4 == sm1.size());
    REQUIRE(nullptr != sm1.data());
    REQUIRE("/MNO" == sm1.name());
    sm1.lock();
    uint32_t *data = reinterpret_cast<uint32_t*>(sm1.data());
    REQUIRE(0 == *data);
    sm1.unlock();

    // Spawning first thread to attach and change data.
    std::thread producerA([](){
        cluon::SharedMemory inner_sm1{"/MNO"};
        REQUIRE(inner_sm1.valid());
        REQUIRE(nullptr != inner_sm1.data());
        REQUIRE("/MNO" == inner_sm1.name());

        inner_sm1.wait();

        inner_sm1.lock();
        uint32_t *inner_data = reinterpret_cast<uint32_t*>(inner_sm1.data());
        *inner_data += 1;
        inner_sm1.unlock();
    });

    // Spawning second thread to attach and change data.
    std::thread producerB([](){
        cluon::SharedMemory inner_sm2{"/MNO"};
        REQUIRE(inner_sm2.valid());
        REQUIRE(nullptr != inner_sm2.data());
        REQUIRE("/MNO" == inner_sm2.name());

        inner_sm2.wait();

        inner_sm2.lock();
        uint32_t *inner_data = reinterpret_cast<uint32_t*>(inner_sm2.data());
        *inner_data += 2;
        inner_sm2.unlock();
    });

    // Wait for threads to come alive.
    using namespace std::literals::chrono_literals; // NOLINT
    std::this_thread::sleep_for(100ms);

    sm1.notifyAll();

    uint32_t tmp{0};
    do {
        sm1.lock();
        tmp = *(reinterpret_cast<uint32_t*>(sm1.data()));
        sm1.unlock();

        std::this_thread::sleep_for(10ms);
    } while (0 == tmp);

    producerA.join();
    producerB.join();

    REQUIRE(3 == tmp);
}

