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

#include "cluon/ToCSVVisitor.hpp"

#include <iomanip>
#include <sstream>

namespace cluon {

ToCSVVisitor::ToCSVVisitor(char delimiter, bool withHeader, const std::map<uint32_t, bool> &mask) noexcept
    : m_mask(mask)
    , m_prefix("")
    , m_delimiter(delimiter)
    , m_withHeader(withHeader)
    , m_isNested(false) {}

ToCSVVisitor::ToCSVVisitor(const std::string &prefix, char delimiter, bool withHeader, bool isNested) noexcept
    : m_prefix(prefix)
    , m_delimiter(delimiter)
    , m_withHeader(withHeader)
    , m_isNested(isNested) {}

void ToCSVVisitor::clear() noexcept {
    m_bufferHeader.str("");
    m_bufferValues.str("");
    m_fillHeader = true;
}

std::string ToCSVVisitor::csv() const noexcept {
    std::stringstream tmp;
    if (m_withHeader) {
        tmp << m_bufferHeader.str();
    }
    tmp << m_bufferValues.str();
    const std::string retVal{tmp.str()};
    return retVal;
}

void ToCSVVisitor::preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void ToCSVVisitor::postVisit() noexcept {
    if (m_fillHeader) {
        m_bufferHeader << (m_isNested ? "" : "\n");
    }
    m_fillHeader = false;
    m_bufferValues << (m_isNested ? "" : "\n");
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << +v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << +v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << v << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << std::setprecision(7) << v << std::setprecision(6) << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << std::setprecision(11) << v << std::setprecision(6) << m_delimiter;
    }
}

void ToCSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        if (m_fillHeader) {
            m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
        }
        m_bufferValues << '\"' << v << '\"' << m_delimiter;
    }
}

} // namespace cluon
