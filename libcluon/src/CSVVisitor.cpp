/*
 * Copyright (C) 2017  Christian Berger
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

#include "cluon/CSVVisitor.hpp"
#include <iomanip>
#include <sstream>

namespace cluon {

CSVVisitor::CSVVisitor(const std::string &prefix, char delimiter, bool withHeader, bool isNested) noexcept 
    : m_prefix(prefix)
    , m_delimiter(delimiter)
    , m_withHeader(withHeader)
    , m_isNested(isNested)
{}

void CSVVisitor::clear() noexcept {
    m_bufferHeader.str("");
    m_bufferValues.str("");
    m_fillHeader = true;
}

std::string CSVVisitor::csv() const noexcept {
    std::stringstream tmp;
    if (m_withHeader) {
        tmp << m_bufferHeader.str();
    }
    tmp << m_bufferValues.str();
    const std::string retVal{tmp.str()};
    return retVal;
}

void CSVVisitor::preVisit(uint32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void CSVVisitor::postVisit() noexcept {
    if (m_fillHeader) {
        m_bufferHeader << (m_isNested ? "" : "\n");
    }
    m_fillHeader = false;
    m_bufferValues << (m_isNested ? "" : "\n");
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << +v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << +v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << v << m_delimiter;
}

void CSVVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;
    if (m_fillHeader) {
        m_bufferHeader << m_prefix << (!m_prefix.empty() ? "." : "") << name << m_delimiter;
    }
    m_bufferValues << "\"" << v << "\"" << m_delimiter;
}

} // namespace cluon
