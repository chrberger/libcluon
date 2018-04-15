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
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
#endif
// clang-format on

namespace cluon {

SharedMemory::SharedMemory(const std::string &name, uint32_t size) noexcept {
    (void)size;
    if (!name.empty()) {
        const std::string n{name.substr(0, (name.size() > 255 ? 255 : name.size()))};
        if ('/' != n[0]) {
            m_name = "/";
        }
        m_name += n;
        if (m_name.size() > 255) {
            m_name = m_name.substr(0, 255);
        }

#ifndef WIN32
    m_fd = ::shm_open(m_name.c_str(), O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
//    ftruncate(fd_sync, sizeof(shared_memory_sync));
//    void* addr_sync = mmap(0, sizeof(shared_memory_sync), PROT_READ|PROT_WRITE, MAP_SHARED, fd_sync, 0);
//    shared_memory_sync* p_sync = static_cast<shared_memory_sync*> (addr_sync);
#endif
    }
}

SharedMemory::~SharedMemory() noexcept {
#ifndef WIN32
    ::shm_unlink(m_name.c_str());
#endif
}

void SharedMemory::lock() noexcept {}

void SharedMemory::unlock() noexcept {}

uint32_t SharedMemory::size() const noexcept {
    return m_size;
}

const std::string SharedMemory::name() const noexcept {
    return m_name;
}

bool SharedMemory::valid() noexcept {
    bool valid{-1 != m_fd};
    {
        lock();
        valid = (nullptr != m_sharedMemory);
        unlock();
    }
    return (0 < m_size) && valid;
}

} // namespace cluon
