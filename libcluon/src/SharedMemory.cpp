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

#include "cluon/SharedMemory.hpp"

// clang-format off
#ifndef WIN32
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif
// clang-format on

#include <cstring>
#include <iostream>

namespace cluon {

SharedMemory::SharedMemory(const std::string &name, uint32_t size) noexcept 
    : m_size(size) {
    if (!name.empty()) {
        constexpr int MAX_LENGTH_NAME{254};
        const std::string n{name.substr(0, (name.size() > MAX_LENGTH_NAME ? MAX_LENGTH_NAME : name.size()))};
        if ('/' != n[0]) {
            m_name = "/";
        }
        m_name += n;
        if (m_name.size() > MAX_LENGTH_NAME) {
            m_name = m_name.substr(0, MAX_LENGTH_NAME);
        }

        // If size is greater than 0, the caller wants to create a new shared
        // memory area. Otherwise, the caller wants to open an existing shared memory.
        int flags = O_RDWR;
        if (0 < size) {
            flags |= O_CREAT|O_EXCL;
        }

#ifndef WIN32
        m_fd = ::shm_open(m_name.c_str(), flags, S_IRUSR|S_IWUSR);
        if (-1 == m_fd) {
            std::cerr << "[cluon::SharedMemory] Failed to open shared memory '" << m_name <<"': " << ::strerror(errno) << " (" << errno << ")" << std::endl;
        }
        else {
            bool retVal{true};

            // When creating a shared memory segment, truncate it.
            if (0 < m_size) {
                retVal = (0 == ::ftruncate(m_fd, sizeof(SharedMemoryHeader) + m_size));
                if (!retVal) {
                    std::cerr << "[cluon::SharedMemory] Failed to ftruncate '" << m_name <<"': " << ::strerror(errno) << " (" << errno << ")" << std::endl;
                }
            }

            // Accessing shared memory segment.
            if (retVal) {
                // On opening (i.e., NOT creating) a shared memory segment, m_size is still 0 and we need to figure out the size first.
                m_sharedMemory = static_cast<char*>(::mmap(0, sizeof(SharedMemoryHeader) + m_size, PROT_READ|PROT_WRITE, MAP_SHARED, m_fd, 0));
                if ( MAP_FAILED != m_sharedMemory) {
                    m_sharedMemoryHeader = reinterpret_cast<SharedMemoryHeader*>(m_sharedMemory);

                    // On creating (i.e., NOT opening) a shared memory segment, erase newly created memory segment.
                    if (0 < m_size) {
                        // Set shared memory area to 0.
                        ::memset(m_sharedMemory, 0, sizeof(SharedMemoryHeader) + m_size);

                        // Create shared mutex therein.
                        pthread_mutexattr_t mutexAttribute;
                        ::pthread_mutexattr_init(&mutexAttribute);
                        ::pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
                        ::pthread_mutex_init(&(m_sharedMemoryHeader->__mutex), &mutexAttribute);
                        ::pthread_mutexattr_destroy(&mutexAttribute);

                        // Store correct size in shared memory.
                        m_sharedMemoryHeader->__size = m_size;
                    }
                    else {
                        // Read size as we are attaching to an existing shared memory.
                        m_size = m_sharedMemoryHeader->__size;

                        // Now, as we know the real size, unmap the first mapping that did not the size.
                        if ( (nullptr != m_sharedMemory) && ::munmap(m_sharedMemory, sizeof(SharedMemoryHeader)) ) {
                            std::cerr << "[cluon::SharedMemory] Failed to munmap shared memory: " << ::strerror(errno) << " (" << errno << ")" << std::endl;
                        }

                        // Invalidate all pointers.
                        m_sharedMemory = nullptr;
                        m_sharedMemoryHeader = nullptr;

                        // Re-map with the correct size parameter.
                        m_sharedMemory = static_cast<char*>(::mmap(0, sizeof(SharedMemoryHeader) + m_size, PROT_READ|PROT_WRITE, MAP_SHARED, m_fd, 0));
                        if ( MAP_FAILED != m_sharedMemory) {
                            m_sharedMemoryHeader = reinterpret_cast<SharedMemoryHeader*>(m_sharedMemory);
                        }
                    }
                }
                else {
                    std::cerr << "[cluon::SharedMemory] Failed to mmap '" << m_name <<"': " << ::strerror(errno) << " (" << errno << ")" << std::endl;
                }

                // If the shared memory segment is correctly available, store the pointer for the user data.
                if ( MAP_FAILED != m_sharedMemory) {
                    m_userAccessibleSharedMemory = m_sharedMemory + sizeof(SharedMemoryHeader);
                }
            }
        }
#endif
    }
}

SharedMemory::~SharedMemory() noexcept {
#ifndef WIN32
    if (nullptr != m_sharedMemoryHeader) {
        ::pthread_mutex_destroy(&(m_sharedMemoryHeader->__mutex));
    }
    if ( (nullptr != m_sharedMemory) && ::munmap(m_sharedMemory, sizeof(SharedMemoryHeader) + m_size) ) {
        std::cerr << "[cluon::SharedMemory] Failed to munmap shared memory: " << ::strerror(errno) << " (" << errno << ")" << std::endl;
    }
    if ( (-1 != m_fd) && (-1 == ::shm_unlink(m_name.c_str()) && (ENOENT != errno)) ) {
        std::cerr << "[cluon::SharedMemory] Failed to unlink shared memory: " << ::strerror(errno) << " (" << errno << ")" << std::endl;
    }
#endif
}

void SharedMemory::lock() noexcept {
    if (nullptr != m_sharedMemoryHeader) {
        ::pthread_mutex_lock(&(m_sharedMemoryHeader->__mutex));
    }
}

void SharedMemory::unlock() noexcept {
    if (nullptr != m_sharedMemoryHeader) {
        ::pthread_mutex_unlock(&(m_sharedMemoryHeader->__mutex));
    }
}

char* SharedMemory::data() noexcept {
    return m_userAccessibleSharedMemory;
}

uint32_t SharedMemory::size() const noexcept {
    return m_size;
}

const std::string SharedMemory::name() const noexcept {
    return m_name;
}

bool SharedMemory::valid() noexcept {
    bool valid{-1 != m_fd};
    valid &= (nullptr != m_sharedMemory);
    valid &= (0 < m_size);
    return valid;
}

} // namespace cluon
