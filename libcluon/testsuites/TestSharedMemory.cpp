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

#include <cstdlib>
#include <chrono>
#include <fstream>
#include <thread>

TEST_CASE("Trying to open SharedMemory with empty name (on non-Win32: POSIX).") {
#ifndef WIN32
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=1"));
#endif
    {
        cluon::SharedMemory sm1{""};
        REQUIRE(!sm1.valid());
        REQUIRE(0 == sm1.size());
        REQUIRE(nullptr == sm1.data());
        REQUIRE(sm1.name().empty());
    }
#ifndef WIN32
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=1" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to open SharedMemory with name without leading / (on non-Win32: POSIX).") {
#ifndef WIN32
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=1"));
#endif
    {
        cluon::SharedMemory sm1{"ABC"};
        REQUIRE(!sm1.valid());
        REQUIRE(0 == sm1.size());
        REQUIRE(nullptr == sm1.data());
        REQUIRE("/ABC" == sm1.name());
    }
#ifndef WIN32
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=1" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to open SharedMemory with name without leading / and too long name > 255 (on non-Win32: POSIX).") {
#ifndef WIN32
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=1"));
#endif
    {
        const std::string NAME{"Vlrel3r6cZeWaRsWgvCWfAHtpPKX56fSgNYNM5bMjEcBnuiMOG3g4YJ4Y9KbPcNyes45xPI9jD5FjxEB1GR9WqaWmyqdH6po1O6is2aDecMe8GGlwqkVJtWH5YwlCYgoJ1E"
                               "iQhqIUVfzp56IY00J6lXJS0uVJrpcMIZuiCsTGTQDG0vPC2EkdbMxe9BPV6a8BnMMumnGKYcqFxiCGrv1SVtLw40zLXTuelQQHiPCFANYlISyhRPt456PMNm7AQJUMHA5"};
    #ifdef WIN32
        const std::string NAME_254 = "/" + NAME.substr(0, MAX_PATH - 1);
    #else
        const std::string NAME_254 = "/" + NAME.substr(0, 253);
    #endif
        cluon::SharedMemory sm1{NAME};
        REQUIRE(!sm1.valid());
        REQUIRE(nullptr == sm1.data());
        REQUIRE(NAME_254 == sm1.name());
    }
#ifndef WIN32
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=1" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name (on non-Win32: POSIX).") {
#ifndef WIN32
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=1"));
#endif
    {
        cluon::SharedMemory sm1{"/DEF", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/DEF" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        *data          = 12345;
        sm1.unlock();

        sm1.lock();
        uint32_t *data2 = reinterpret_cast<uint32_t *>(sm1.data());
        uint32_t tmp    = *data2;
        sm1.unlock();
        REQUIRE(12345 == tmp);
    }
#ifndef WIN32
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=1" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name and one reader and one writer instance (on non-Win32: POSIX).") {
#ifndef WIN32
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=1"));
#endif
    {
        cluon::SharedMemory sm1{"/DEF2", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/DEF2" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        *data          = 12345;
        sm1.unlock();

        {
            cluon::SharedMemory sm2{"/DEF2"};
            REQUIRE(sm2.valid());
            REQUIRE(4 == sm2.size());
            REQUIRE(nullptr != sm2.data());
            REQUIRE("/DEF2" == sm2.name());
            sm2.lock();
            uint32_t *data2 = reinterpret_cast<uint32_t *>(sm2.data());
            uint32_t tmp    = *data2;
            sm2.unlock();
            REQUIRE(12345 == tmp);
        }
    }
#ifndef WIN32
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=1" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name and separate thread to produce data for shared memory (on non-Win32: POSIX).") {
#ifndef WIN32
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=1"));
#endif
    {
        cluon::SharedMemory sm1{"/GHI", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/GHI" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        REQUIRE(0 == *data);
        sm1.unlock();

        // Spawning thread to attach and change data.
        std::thread producer([]() {
            cluon::SharedMemory inner_sm1{"/GHI"};
            REQUIRE(inner_sm1.valid());
            REQUIRE(nullptr != inner_sm1.data());
            REQUIRE("/GHI" == inner_sm1.name());
            inner_sm1.lock();
            uint32_t *inner_data = reinterpret_cast<uint32_t *>(inner_sm1.data());
            REQUIRE(0 == *inner_data);
            *inner_data = 54321;
            REQUIRE(54321 == *inner_data);
            inner_sm1.unlock();
        });

        uint32_t tmp{0};
        do {
            sm1.lock();
            tmp = *(reinterpret_cast<uint32_t *>(sm1.data()));
            sm1.unlock();

            using namespace std::literals::chrono_literals; // NOLINT
            std::this_thread::sleep_for(10ms);
        } while (0 == tmp);

        producer.join();

        REQUIRE(54321 == tmp);
    }
#ifndef WIN32
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=1" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name and separate thread to produce data for shared memory with condition variable for synchronization (on non-Win32: POSIX).") {
#ifndef WIN32
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=1"));
#endif
    {
        cluon::SharedMemory sm1{"/JKL", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/JKL" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        REQUIRE(0 == *data);
        sm1.unlock();

        // Spawning thread to attach and change data.
        std::thread producer([]() {
            cluon::SharedMemory inner_sm1{"/JKL"};
            REQUIRE(inner_sm1.valid());
            REQUIRE(nullptr != inner_sm1.data());
            REQUIRE("/JKL" == inner_sm1.name());
            inner_sm1.lock();
            uint32_t *inner_data = reinterpret_cast<uint32_t *>(inner_sm1.data());
            REQUIRE(0 == *inner_data);
            *inner_data = 23456;
            REQUIRE(23456 == *inner_data);
            inner_sm1.unlock();

            inner_sm1.notifyAll();
        });

        sm1.wait();
        producer.join();

        sm1.lock();
        uint32_t tmp = *(reinterpret_cast<uint32_t *>(sm1.data()));
        sm1.unlock();

        REQUIRE(23456 == tmp);
    }
#ifndef WIN32
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=1" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name and two separate threads to produce data for shared memory with condition variable for synchronization (on non-Win32: POSIX).") {
#ifndef WIN32
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=1"));
#endif
    {
        cluon::SharedMemory sm1{"/MNO", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/MNO" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        REQUIRE(0 == *data);
        sm1.unlock();

        // Spawning first thread to attach and change data.
        std::thread producerA([]() {
            cluon::SharedMemory inner_sm1{"/MNO"};
            REQUIRE(inner_sm1.valid());
            REQUIRE(nullptr != inner_sm1.data());
            REQUIRE("/MNO" == inner_sm1.name());

            inner_sm1.wait();

            inner_sm1.lock();
            uint32_t *inner_data = reinterpret_cast<uint32_t *>(inner_sm1.data());
            *inner_data += 1;
            inner_sm1.unlock();
        });

        // Spawning second thread to attach and change data.
        std::thread producerB([]() {
            cluon::SharedMemory inner_sm2{"/MNO"};
            REQUIRE(inner_sm2.valid());
            REQUIRE(nullptr != inner_sm2.data());
            REQUIRE("/MNO" == inner_sm2.name());

            inner_sm2.wait();

            inner_sm2.lock();
            uint32_t *inner_data = reinterpret_cast<uint32_t *>(inner_sm2.data());
            *inner_data += 2;
            inner_sm2.unlock();
        });

        // Wait for threads to come alive.
        using namespace std::literals::chrono_literals; // NOLINT
        std::this_thread::sleep_for(100ms);

        sm1.notifyAll();

        producerA.join();
        producerB.join();

        // Finally read the last value.
        uint32_t tmp = *(reinterpret_cast<uint32_t *>(sm1.data()));
        REQUIRE(3 == tmp);
    }
#ifndef WIN32
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=1" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory that existed before to remove it (only POSIX).") {
#ifdef __linux__
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=1"));
    {
        std::fstream fout("/dev/shm/PQRABCDEFGHI", std::ios::out | std::ios::trunc);
        fout.close();

        cluon::SharedMemory sm1{"/PQRABCDEFGHI", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/PQRABCDEFGHI" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        *data          = 34567;
        sm1.unlock();

        sm1.lock();
        uint32_t *data2 = reinterpret_cast<uint32_t *>(sm1.data());
        uint32_t tmp    = *data2;
        sm1.unlock();
        REQUIRE(34567 == tmp);
    }
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=1" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Tests for SysV implementation.
TEST_CASE("Trying to open SharedMemory with empty name (SySV).") {
#ifdef __linux__
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=0"));
    {
        cluon::SharedMemory sm1{""};
        REQUIRE(!sm1.valid());
        REQUIRE(0 == sm1.size());
        REQUIRE(nullptr == sm1.data());
        REQUIRE(sm1.name().empty());
    }
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=0" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to open SharedMemory with name without leading / (SySV).") {
#ifdef __linux__
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=0"));
    {
        cluon::SharedMemory sm1{"ABC"};
        REQUIRE(!sm1.valid());
        REQUIRE(0 == sm1.size());
        REQUIRE(nullptr == sm1.data());
        REQUIRE("/tmp/ABC" == sm1.name());
    }
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=0" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to open SharedMemory with name without leading / and too long name > 255 (SySV).") {
#ifdef __linux__
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=0"));
    {
        const std::string NAME{"Vlrel3r6cZeWaRsWgvCWfAHtpPKX56fSgNYNM5bMjEcBnuiMOG3g4YJ4Y9KbPcNyes45xPI9jD5FjxEB1GR9WqaWmyqdH6po1O6is2aDecMe8GGlwqkVJtWH5YwlCYgoJ1E"
                               "iQhqIUVfzp56IY00J6lXJS0uVJrpcMIZuiCsTGTQDG0vPC2EkdbMxe9BPV6a8BnMMumnGKYcqFxiCGrv1SVtLw40zLXTuelQQHiPCFANYlISyhRPt456PMNm7AQJUMHA5"};
        const std::string NAME_254 = "/tmp/" + NAME.substr(0, 249);
        cluon::SharedMemory sm1{NAME};
        REQUIRE(!sm1.valid());
        REQUIRE(nullptr == sm1.data());
        REQUIRE(NAME_254 == sm1.name());
    }
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=0" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name (SySV).") {
#ifdef __linux__
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=0"));
    {
        cluon::SharedMemory sm1{"/DEF", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/tmp/DEF" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        *data          = 12345;
        sm1.unlock();

        sm1.lock();
        uint32_t *data2 = reinterpret_cast<uint32_t *>(sm1.data());
        uint32_t tmp    = *data2;
        sm1.unlock();
        REQUIRE(12345 == tmp);
    }
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=0" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name and one reader and one writer instance (SySV).") {
#ifdef __linux__
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=0"));
    {
        cluon::SharedMemory sm1{"/DEF", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/tmp/DEF" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        *data          = 12345;
        sm1.unlock();

        {
            cluon::SharedMemory sm2{"/DEF"};
            REQUIRE(sm2.valid());
            REQUIRE(4 == sm2.size());
            REQUIRE(nullptr != sm2.data());
            REQUIRE("/tmp/DEF" == sm2.name());
            sm2.lock();
            uint32_t *data2 = reinterpret_cast<uint32_t *>(sm2.data());
            uint32_t tmp    = *data2;
            sm2.unlock();
            REQUIRE(12345 == tmp);
        }
    }
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=0" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name and separate thread to produce data for shared memory (SySV).") {
#ifdef __linux__
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=0"));
    {
        cluon::SharedMemory sm1{"/GHI", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/tmp/GHI" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        REQUIRE(0 == *data);
        sm1.unlock();

        // Spawning thread to attach and change data.
        std::thread producer([]() {
            cluon::SharedMemory inner_sm1{"/GHI"};
            REQUIRE(inner_sm1.valid());
            REQUIRE(nullptr != inner_sm1.data());
            REQUIRE("/tmp/GHI" == inner_sm1.name());
            inner_sm1.lock();
            uint32_t *inner_data = reinterpret_cast<uint32_t *>(inner_sm1.data());
            REQUIRE(0 == *inner_data);
            *inner_data = 54321;
            REQUIRE(54321 == *inner_data);
            inner_sm1.unlock();
        });

        uint32_t tmp{0};
        do {
            sm1.lock();
            tmp = *(reinterpret_cast<uint32_t *>(sm1.data()));
            sm1.unlock();

            using namespace std::literals::chrono_literals; // NOLINT
            std::this_thread::sleep_for(10ms);
        } while (0 == tmp);

        producer.join();

        REQUIRE(54321 == tmp);
    }
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=0" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name and separate thread to produce data for shared memory with condition variable for synchronization (SySV).") {
#ifdef __linux__
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=0"));
    {
        cluon::SharedMemory sm1{"/JKL", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/tmp/JKL" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        REQUIRE(0 == *data);
        sm1.unlock();

        // Spawning thread to attach and change data.
        std::thread producer([]() {
            cluon::SharedMemory inner_sm1{"/JKL"};
            REQUIRE(inner_sm1.valid());
            REQUIRE(nullptr != inner_sm1.data());
            REQUIRE("/tmp/JKL" == inner_sm1.name());
            inner_sm1.lock();
            uint32_t *inner_data = reinterpret_cast<uint32_t *>(inner_sm1.data());
            REQUIRE(0 == *inner_data);
            *inner_data = 23456;
            REQUIRE(23456 == *inner_data);
            inner_sm1.unlock();

            inner_sm1.notifyAll();
        });

        sm1.wait();
        producer.join();

        sm1.lock();
        uint32_t tmp = *(reinterpret_cast<uint32_t *>(sm1.data()));
        sm1.unlock();

        REQUIRE(23456 == tmp);
    }
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=0" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

TEST_CASE("Trying to create SharedMemory with correct name and two separate threads to produce data for shared memory with condition variable for synchronization (SySV).") {
#ifdef __linux__
    const char *CLUON_SHAREDMEMORY_POSIX = getenv("CLUON_SHAREDMEMORY_POSIX");
    bool usePOSIX = ( (nullptr != CLUON_SHAREDMEMORY_POSIX) && (CLUON_SHAREDMEMORY_POSIX[0] == '1') );
    putenv(const_cast<char *>("CLUON_SHAREDMEMORY_POSIX=0"));
    {
        cluon::SharedMemory sm1{"/MNO", 4};
        REQUIRE(sm1.valid());
        REQUIRE(4 == sm1.size());
        REQUIRE(nullptr != sm1.data());
        REQUIRE("/tmp/MNO" == sm1.name());
        sm1.lock();
        uint32_t *data = reinterpret_cast<uint32_t *>(sm1.data());
        REQUIRE(0 == *data);
        sm1.unlock();

        // Spawning first thread to attach and change data.
        std::thread producerA([]() {
            cluon::SharedMemory inner_sm1{"/MNO"};
            REQUIRE(inner_sm1.valid());
            REQUIRE(nullptr != inner_sm1.data());
            REQUIRE("/tmp/MNO" == inner_sm1.name());

            inner_sm1.wait();

            inner_sm1.lock();
            uint32_t *inner_data = reinterpret_cast<uint32_t *>(inner_sm1.data());
            *inner_data += 1;
            inner_sm1.unlock();
        });

        // Spawning second thread to attach and change data.
        std::thread producerB([]() {
            cluon::SharedMemory inner_sm2{"/MNO"};
            REQUIRE(inner_sm2.valid());
            REQUIRE(nullptr != inner_sm2.data());
            REQUIRE("/tmp/MNO" == inner_sm2.name());

            inner_sm2.wait();

            inner_sm2.lock();
            uint32_t *inner_data = reinterpret_cast<uint32_t *>(inner_sm2.data());
            *inner_data += 2;
            inner_sm2.unlock();
        });

        // Wait for threads to come alive.
        using namespace std::literals::chrono_literals; // NOLINT
        std::this_thread::sleep_for(100ms);

        sm1.notifyAll();

        producerA.join();
        producerB.join();

        // Finally read the last value.
        uint32_t tmp = *(reinterpret_cast<uint32_t *>(sm1.data()));
        REQUIRE(3 == tmp);
    }
    putenv(const_cast<char*>((usePOSIX ? "CLUON_SHAREDMEMORY_POSIX=0" : "CLUON_SHAREDMEMORY_POSIX=0")));
#endif
}

