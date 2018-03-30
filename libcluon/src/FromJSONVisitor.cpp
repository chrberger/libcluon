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

#include "cluon/FromJSONVisitor.hpp"
#include "cluon/stringtoolbox.hpp"

#include <cstring>
#include <array>
#include <regex>
#include <sstream>
#include <vector>

#include <iostream>

namespace cluon {

FromJSONVisitor::FromJSONVisitor() noexcept
    : m_keyValues{m_data} {}

FromJSONVisitor::FromJSONVisitor(std::map<std::string, FromJSONVisitor::JSONKeyValue> &preset) noexcept
    : m_keyValues{preset} {}

std::map<std::string, FromJSONVisitor::JSONKeyValue> FromJSONVisitor::readKeyValues(std::string &input, int indent) noexcept {
    (void)indent;
    const std::string MATCH_JSON = R"((?:\"|\')(?:[^"]*)(?:\"|\')(?=:)(?:\:\s*)(?:\"|\')?(?:true|false|[\-]{0,1}[0-9]+[\.][0-9]+|[\-]{0,1}[0-9]+|[0-9a-zA-Z\+\-\,\.\$\ \=]*)(?:\"|\')?)";

    std::map<std::string, FromJSONVisitor::JSONKeyValue> result;
    std::string oldInput;
    try {
        std::smatch m;
        std::string keyOfNestedObject;
        do {
            std::regex_search(input, m, std::regex(MATCH_JSON));
            std::string p{m.prefix()};

//std::cout << "P = '" << p << "'" << std::endl;
            if (p.size() > 1 && p.at(0) == '"' && p.at(1) == '}') {
//                std::cout << "End nested object" << std::endl;
                indent--;
            }
            else if (p.size() > 0 && p.at(0) == '}') {
//                std::cout << "End nested object" << std::endl;
                indent--;
            }

            if (m.size() > 0) {
                std::string match{m[0]};
//std::cout << "M = '" << match << "'" << std::endl;

                std::vector<std::string> retVal = stringtoolbox::split(match, ':');
//std::cout << "Si=" << retVal.size() << std::endl;
                if ( (retVal.size() == 1) || ( (retVal.size() == 2) && (stringtoolbox::trim(retVal[1]).size() == 0) ) ) {
                    keyOfNestedObject = stringtoolbox::trim(retVal[0]);
//std::cout << "Nested object " << keyOfNestedObject << std::endl;

                    std::string suf(m.suffix());
                    suf = stringtoolbox::trim(suf);
                    if (!suf.empty()) {
//std::cout << "S_nested = '" << suf << "'" << std::endl;
                    }
                    input = suf;

                    indent++;
                    auto r = readKeyValues(input, indent);

                    JSONKeyValue kv;
                    kv.m_key = stringtoolbox::split(keyOfNestedObject, '"')[0];
                    kv.m_type = JSONConstants::OBJECT;
                    kv.m_value = r;

                    result[kv.m_key] = kv;

                    keyOfNestedObject = "";
                }
                if ( (retVal.size() == 2) && (stringtoolbox::trim(retVal[1]).size() > 0) ) {
                    auto e = std::make_pair(stringtoolbox::trim(retVal[0]), stringtoolbox::trim(retVal[1]));

//for(int i = 0; i < indent; i++) std::cout << " ";
//std::cout << e.first << "=" << e.second << std::endl;

                    JSONKeyValue kv;
                    kv.m_key = stringtoolbox::split(e.first, '"')[0];

                    if ( (e.second.size() > 0) && (e.second.at(0) == '"') ) {
//std::cout << "Found string" << std::endl;
                        kv.m_type = JSONConstants::STRING;
                        kv.m_value = std::string(e.second).substr(1);
                    }
                    else if ( (e.second.size() > 0) && ( (e.second == "false") || (e.second == "true") ) ) {
//std::cout << "Found boolean" << std::endl;
                        kv.m_value = e.second == "true";

                        kv.m_type = (e.second == "true" ? JSONConstants::IS_TRUE : JSONConstants::IS_FALSE);
                    }
                    else {
                        kv.m_type = JSONConstants::NUMBER;
                        std::stringstream tmp(e.second);
                        double d;
                        tmp >> d;
//std::cout << "Found number: " << d << std::endl;
                        kv.m_value = d;
                    }
//std::cout << "key = " << "'" << kv.m_key << "'" << std::endl;

                    result[kv.m_key] = kv;

                    std::string suf(m.suffix());
                    suf = stringtoolbox::trim(suf);
                    if (!suf.empty()) {
//std::cout << "S = '" << suf << "'" << std::endl;
                    }
                    oldInput = input;
                    input = suf;
                }
            }
        } while (!m.empty() && (oldInput != input));
    } catch (std::regex_error &) {
    } catch (std::bad_cast &) {
    }
//std::cout << std::endl;
    return result;
}

void FromJSONVisitor::decodeFrom(std::istream &in) noexcept {
    m_keyValues.clear();

    std::stringstream sstr;
    while (in.good()) {
        uint8_t c = static_cast<uint8_t>(in.get());
        sstr.write(reinterpret_cast<char*>(&c), sizeof(char));
    }
    std::string s{sstr.str()};

    // Remove whitespace characters like newline, carriage return, or tab.
    s.erase(std::remove_if( s.begin(), s.end(), [](char c){ return (c =='\r' || c =='\t' || c == '\n');}), s.end() );

    // Parse JSON from in.
    m_keyValues = readKeyValues(s, 0);
}

std::string FromJSONVisitor::decodeBase64(const std::string &input) const noexcept {
    const std::string ALPHABET{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

    uint32_t index{0};
    char *decoded = new char[input.size() * 3 / 4];
    char counter{0};
    std::array<char, 4> buffer;

    for (uint32_t i{0}; i < input.size(); i++) {
        char c;
        for (c = 0 ; c < 64 && (ALPHABET.at(c) != input.at(i)); c++);

        buffer[counter++] = c;
        if (4 == counter) {
            decoded[index++] = (buffer[0] << 2) + (buffer[1] >> 4);
            if (64 != buffer[2]) {
                decoded[index++] = (buffer[1] << 4) + (buffer[2] >> 2);
            }
            if (64 != buffer[3]) {
                decoded[index++] = (buffer[2] << 6) + buffer[3];
            }
            counter = 0;
        }
    }

    decoded[index] = 0;
    std::string retVal(decoded, index);
    delete [] decoded;
    return retVal;
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
            }
            else if (JSONConstants::IS_TRUE == m_keyValues[name].m_type) {
                v = true;
            }
            else if (JSONConstants::NUMBER == m_keyValues[name].m_type) {
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
            v = decodeBase64(tmp);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

} // namespace cluon
