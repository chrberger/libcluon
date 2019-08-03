/*
 * Copyright (C) 2017-2019  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/FromJSONVisitor.hpp"
#include "cluon/stringtoolbox.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <iterator>
#include <regex>
#include <sstream>
#include <vector>

#include <iostream>
namespace cluon {

FromJSONVisitor::FromJSONVisitor() noexcept
    : m_keyValues{m_data} {}

FromJSONVisitor::FromJSONVisitor(std::map<std::string, FromJSONVisitor::JSONKeyValue> &preset) noexcept
    : m_keyValues{preset} {}

std::map<std::string, FromJSONVisitor::JSONKeyValue> FromJSONVisitor::readKeyValues(std::string &input) noexcept {
    const std::string MATCH_JSON
        = R"((?:\"|\')(?:[^"]*)(?:\"|\')(?=:)(?:\:\s*)(?:\"|\')?(?:true|false|[\-]{0,1}[0-9]+[\.][0-9]+|[\-]{0,1}[0-9]+|[0-9a-zA-Z\+\-\,\.\$\ \=]*)(?:\"|\')?)";

    std::map<std::string, FromJSONVisitor::JSONKeyValue> result;
    std::string oldInput;
    try {
        std::smatch m;
        do {
            std::regex_search(input, m, std::regex(MATCH_JSON));

            if (m.size() > 0) {
                std::string match{m[0]};
                std::vector<std::string> retVal = stringtoolbox::split(match, ':');
                if ( (retVal.size() == 2) && (stringtoolbox::trim(retVal[1]).size() == 0) ) {
                    std::string keyOfNestedObject{stringtoolbox::trim(retVal[0])};
                    keyOfNestedObject = stringtoolbox::split(keyOfNestedObject, '"')[1];
                    {
                        std::string suffix(m.suffix());
                        suffix   = stringtoolbox::trim(suffix);
                        oldInput = input;
                        input    = suffix;
                    }

                    auto mapOfNestedValues = readKeyValues(input);

                    JSONKeyValue kv;
                    kv.m_key   = keyOfNestedObject;
                    kv.m_type  = JSONConstants::OBJECT;
                    kv.m_value = mapOfNestedValues;

                    result[keyOfNestedObject] = kv;
                }
                if ( (retVal.size() == 2) && (stringtoolbox::trim(retVal[1]).size() > 0) ) {
                    auto e = std::make_pair(stringtoolbox::trim(retVal[0]), stringtoolbox::trim(retVal[1]));

                    JSONKeyValue kv;
                    kv.m_key = stringtoolbox::split(e.first, '"')[1];

                    if ((e.second.size() > 0) && (e.second.at(0) == '"')) {
                        kv.m_type  = JSONConstants::STRING;
                        kv.m_value = std::string(e.second).substr(1);
                    } else if ((e.second.size() > 0) && ((e.second == "false") || (e.second == "true"))) {
                        kv.m_type  = (e.second == "true" ? JSONConstants::IS_TRUE : JSONConstants::IS_FALSE);
                        kv.m_value = e.second == "true";
                    } else {
                        kv.m_type = JSONConstants::NUMBER;
                        std::stringstream tmp(e.second);
                        double v;
                        tmp >> v;
                        kv.m_value = v;
                    }

                    result[kv.m_key] = kv;

                    {
                        std::string suffix(m.suffix());
                        suffix   = stringtoolbox::trim(suffix);
                        oldInput = input;
                        input    = suffix;
                        if (suffix.size() > 0 && suffix.at(0) == '}') {
                            break; // Nested payload complete; return.
                        }
                    }
                }
            }
        } while (!m.empty() && (oldInput != input));
    } catch (std::regex_error &) { // LCOV_EXCL_LINE
    } catch (std::bad_cast &) {}   // LCOV_EXCL_LINE

    return result;
}

void FromJSONVisitor::decodeFrom(std::istream &in) noexcept {
    m_keyValues.clear();

    std::string s;
    std::istream_iterator<char> it(in), it_end;
    std::copy(it, it_end, std::insert_iterator<std::string>(s, s.begin()));

    // Remove whitespace characters like newline, carriage return, or tab.
    s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return (c == '\r' || c == '\t' || c == '\n'); }), s.end());

    // Parse JSON from in.
    m_keyValues = readKeyValues(s);
}

std::string FromJSONVisitor::decodeBase64(const std::string &input) noexcept {
    const std::string ALPHABET{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
    uint8_t counter{0};
    std::array<char, 4> buffer;
    std::string decoded;
    for (uint32_t i{0}; i < input.size(); i++) {
        char c;
        for (c = 0; c < 64 && (ALPHABET.at(static_cast<uint8_t>(c)) != input.at(i)); c++) {}

        buffer[counter++] = c;
        if (4 == counter) {
            decoded.push_back(static_cast<char>((buffer[0] << 2) + (buffer[1] >> 4)));
            if (64 != buffer[2]) {
                decoded.push_back(static_cast<char>((buffer[1] << 4) + (buffer[2] >> 2)));
            }
            if (64 != buffer[3]) {
                decoded.push_back(static_cast<char>((buffer[2] << 6) + buffer[3]));
            }
            counter = 0;
        }
    }
    return decoded;
}

void FromJSONVisitor::preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void FromJSONVisitor::postVisit() noexcept {}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::IS_FALSE == m_keyValues[name].m_type) {
                v = false;
            } else if (JSONConstants::IS_TRUE == m_keyValues[name].m_type) {
                v = true;
            } else if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = (1 == static_cast<uint32_t>(linb::any_cast<double>(m_keyValues[name].m_value)));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::STRING == m_keyValues[name].m_type) {
                v = linb::any_cast<std::string>(m_keyValues[name].m_value).at(0);
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = static_cast<int8_t>(linb::any_cast<double>(m_keyValues[name].m_value));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = static_cast<uint8_t>(linb::any_cast<double>(m_keyValues[name].m_value));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = static_cast<int16_t>(linb::any_cast<double>(m_keyValues[name].m_value));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = static_cast<uint16_t>(linb::any_cast<double>(m_keyValues[name].m_value));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = static_cast<int32_t>(linb::any_cast<double>(m_keyValues[name].m_value));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = static_cast<uint32_t>(linb::any_cast<double>(m_keyValues[name].m_value));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = static_cast<int64_t>(linb::any_cast<double>(m_keyValues[name].m_value));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = static_cast<uint64_t>(linb::any_cast<double>(m_keyValues[name].m_value));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = static_cast<float>(linb::any_cast<double>(m_keyValues[name].m_value));
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
                v = linb::any_cast<double>(m_keyValues[name].m_value);
            }
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromJSONVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            std::string tmp{linb::any_cast<std::string>(m_keyValues[name].m_value)};
            v = FromJSONVisitor::decodeBase64(tmp);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

} // namespace cluon
