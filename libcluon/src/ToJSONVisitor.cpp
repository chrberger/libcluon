/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/ToJSONVisitor.hpp"

#include <iomanip>
#include <sstream>

namespace cluon {

ToJSONVisitor::ToJSONVisitor(bool withOuterCurlyBraces, const std::map<uint32_t, bool> &mask) noexcept
    : m_withOuterCurlyBraces(withOuterCurlyBraces)
    , m_mask(mask) {}

std::string ToJSONVisitor::json() const noexcept {
    const std::string tmp{m_buffer.str()};
    std::string retVal{"{}"};
    if (2 < tmp.size()) {
        retVal = {(m_withOuterCurlyBraces ? "{" : "") + tmp.substr(0, tmp.size() - 2) + (m_withOuterCurlyBraces ? "}" : "")};
    }
    return retVal;
}

void ToJSONVisitor::preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)longName;
    (void)shortName;
}

void ToJSONVisitor::postVisit() noexcept {}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << v << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << '\"' << v << '\"' << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << +v << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << +v << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << +v << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << +v << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << v << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << v << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << v << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << v << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << std::setprecision(7) << v << std::setprecision(6) << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << std::setprecision(11) << v << std::setprecision(6) << ',' << '\n';
    }
}

void ToJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    if ((0 == m_mask.count(id)) || m_mask[id]) {
        m_buffer << '\"' << name << '\"' << ':' << '\"' << ToJSONVisitor::encodeBase64(v) << '\"' << ',' << '\n';
    }
}

std::string ToJSONVisitor::encodeBase64(const std::string &input) noexcept {
    std::string retVal;

    const std::string ALPHABET{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
    auto length{input.length()};
    uint32_t index{0};
    uint32_t value{0};

    while (length > 2) {
        value = static_cast<uint32_t>(static_cast<unsigned char>(input.at(index++))) << 16;
        value |= static_cast<uint32_t>(static_cast<unsigned char>(input.at(index++))) << 8;
        value |= static_cast<uint32_t>(static_cast<unsigned char>(input.at(index++)));
        retVal += ALPHABET.at((value & 0xFC0000) >> 18);
        retVal += ALPHABET.at((value & 0x3F000) >> 12);
        retVal += ALPHABET.at((value & 0xFC0) >> 6);
        retVal += ALPHABET.at(value & 0x3F);
        length -= 3;
    }
    if (length == 2) {
        value = static_cast<uint32_t>(static_cast<unsigned char>(input.at(index++))) << 16;
        value |= static_cast<uint32_t>(static_cast<unsigned char>(input.at(index++))) << 8;
        retVal += ALPHABET.at((value & 0xFC0000) >> 18);
        retVal += ALPHABET.at((value & 0x3F000) >> 12);
        retVal += ALPHABET.at((value & 0xFC0) >> 6);
        retVal += "=";
    } else if (length == 1) {
        value = static_cast<uint32_t>(static_cast<unsigned char>(input.at(index++))) << 16;
        retVal += ALPHABET.at((value & 0xFC0000) >> 18);
        retVal += ALPHABET.at((value & 0x3F000) >> 12);
        retVal += "==";
    }

    return retVal;
}

} // namespace cluon
