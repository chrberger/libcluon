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

#ifndef CLUON_SHAREDMEMORY_HPP
#define CLUON_SHAREDMEMORY_HPP

#include "cluon/cluon.hpp"

// clang-format off
#ifdef WIN32
    #include <Windows.h>
#else
    #include <pthread.h>
#endif
// clang-format on

#include <cstddef>
#include <cstdint>
#include <string>

namespace cluon {

class LIBCLUON_API SharedMemory {
   private:
    SharedMemory(const SharedMemory &) = delete;
    SharedMemory(SharedMemory &&)      = delete;
    SharedMemory &operator=(const SharedMemory &) = delete;
    SharedMemory &operator=(SharedMemory &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param name Name of the shared memory area; must start with / and must not
     * be longer than NAME_MAX (255) on POSIX or PATH_MAX on WIN32. If the name
     * is missing a leading '/' or is longer than 255, it will be adjusted accordingly.
     * @param size of the shared memory area to create; if size is 0, the class tries to attach to an existing area.
     */
    SharedMemory(const std::string &name, uint32_t size = 0) noexcept;
    ~SharedMemory() noexcept;

    /**
     * This method locks the shared memory area.
     */
    void lock() noexcept;

    /**
     * This method unlocks the shared memory area.
     */
    void unlock() noexcept;

    /**
     * This method waits for being notified from the shared condition.
     */
    void wait() noexcept;

    /**
     * This method notifies all threads waiting on the shared condition.
     */
    void notifyAll() noexcept;

    /**
     * @return Pointer to the raw shared memory or nullptr in case of invalid shared memory.
     */
    char *data() noexcept;

    /**
     * @return The size of the shared memory area.
     */
    uint32_t size() const noexcept;

    /**
     * @return Name the shared memory area.
     */
    const std::string name() const noexcept;

    /**
     * @return True if the shared memory area is existing and usable.
     */
    bool valid() noexcept;

   private:
    std::string m_name{""};
    uint32_t m_size{0};
    char *m_sharedMemory{nullptr};
    char *m_userAccessibleSharedMemory{nullptr};
    bool m_hasOnlyAttachedToSharedMemory{false};

#ifdef WIN32
    HANDLE __conditionEvent{nullptr};
    HANDLE __mutex{nullptr};
    HANDLE __sharedMemory{nullptr};
#else
    int32_t m_fd{ -1 };
    struct SharedMemoryHeader {
        uint32_t __size;
        pthread_mutex_t __mutex;
        pthread_cond_t __condition;
    };
    SharedMemoryHeader *m_sharedMemoryHeader{nullptr};
#endif
};
} // namespace cluon

#endif
