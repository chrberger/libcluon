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

namespace cluon {

SharedMemory::SharedMemory(const std::string &name, uint32_t size) noexcept {
    (void)name;
    (void)size;
}

SharedMemory::~SharedMemory() noexcept {}

void SharedMemory::lock() noexcept {}

void SharedMemory::unlock() noexcept {}

uint32_t SharedMemory::size() const noexcept {
    return m_size;
}

const std::string SharedMemory::name() const noexcept {
    return m_name;
}

bool SharedMemory::valid() noexcept {
    bool valid{false};
    {
        lock();
        valid = (nullptr != m_sharedMemory);
        unlock();
    }
    return (0 < m_size) && valid;
}

} // namespace cluon
