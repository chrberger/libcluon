/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/ToCSVVisitor.hpp"
#include "cluon/ToJSONVisitor.hpp"

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
        m_bufferValues << '\"' << cluon::ToJSONVisitor::encodeBase64(v) << '\"' << m_delimiter;
    }
}

} // namespace cluon
