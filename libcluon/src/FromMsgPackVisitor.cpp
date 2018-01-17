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

#include "cluon/FromMsgPackVisitor.hpp"

#include <cstring>
#include <iostream>
#include <vector>

namespace cluon {

MsgPackConstants FromMsgPackVisitor::getFormatFamily(uint8_t T) noexcept {
    MsgPackConstants formatFamily{MsgPackConstants::UNKNOWN_FORMAT};

    if (static_cast<uint8_t>(MsgPackConstants::IS_FALSE) == T) {
        formatFamily = MsgPackConstants::BOOL_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::IS_TRUE) == T) {
        formatFamily = MsgPackConstants::BOOL_FORMAT;
    }
    else if ( (static_cast<uint8_t>(MsgPackConstants::FIXSTR) <= T) && (static_cast<uint8_t>(MsgPackConstants::FIXSTR_END) > T) ) {
        formatFamily = MsgPackConstants::STR_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::STR8) == T) {
        formatFamily = MsgPackConstants::STR_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::STR16) == T) {
        formatFamily = MsgPackConstants::STR_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::STR32) == T) {
        formatFamily = MsgPackConstants::STR_FORMAT;
    }
    else if ( (static_cast<uint8_t>(MsgPackConstants::FIXMAP) <= T) && (static_cast<uint8_t>(MsgPackConstants::FIXMAP_END) > T) ) {
        formatFamily = MsgPackConstants::MAP_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::MAP16) == T) {
        formatFamily = MsgPackConstants::MAP_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::MAP32) == T) {
        formatFamily = MsgPackConstants::MAP_FORMAT;
    }

    return formatFamily;
}

std::string FromMsgPackVisitor::readString(std::istream &in) noexcept {
    std::string retVal{""};
    if (in.good()) {
        uint8_t c = static_cast<uint8_t>(in.get());
        if (MsgPackConstants::STR_FORMAT == getFormatFamily(c)) {
            // First, search for str opening token.
            uint32_t length{0};
            const uint8_t T = static_cast<uint8_t>(c);
            if ( (static_cast<uint8_t>(MsgPackConstants::FIXSTR) <= T) && (static_cast<uint8_t>(MsgPackConstants::FIXSTR_END) > T) ) {
                length = T - static_cast<uint8_t>(MsgPackConstants::FIXSTR);
            }
            else if (static_cast<uint8_t>(MsgPackConstants::STR8) == T) {
                uint8_t _length{0};
                in.read(reinterpret_cast<char*>(&_length), sizeof(uint8_t));
                length = _length;
            }
            else if (static_cast<uint8_t>(MsgPackConstants::STR16) == T) {
                uint16_t _length{0};
                in.read(reinterpret_cast<char*>(&_length), sizeof(uint16_t));
                length = be16toh(_length);
            }
            else if (static_cast<uint8_t>(MsgPackConstants::STR32) == T) {
                in.read(reinterpret_cast<char*>(&length), sizeof(uint32_t));
                length = be32toh(length);
            }

            if (0 < length) {
                std::vector<char> buffer;
                buffer.reserve(length);
                in.read(&buffer[0], static_cast<std::streamsize>(length));
                retVal = std::string(buffer.data(), length);
            }
        }
    }
    return retVal;
}

std::map<std::string, FromMsgPackVisitor::MsgPackKeyValue> FromMsgPackVisitor::readKeyValues(std::istream &in) noexcept {
    std::map<std::string, FromMsgPackVisitor::MsgPackKeyValue> keyValues;
    while (in.good()) {
        uint8_t c = static_cast<uint8_t>(in.get());
        if (MsgPackConstants::MAP_FORMAT == getFormatFamily(c)) {
            // First, search for map opening token.
            const uint8_t T = static_cast<uint8_t>(c);
            uint32_t tokensToRead{0};
            if ( (static_cast<uint8_t>(MsgPackConstants::FIXMAP) <= T) && (static_cast<uint8_t>(MsgPackConstants::FIXMAP_END) > T) ) {
                tokensToRead = T - static_cast<uint8_t>(MsgPackConstants::FIXMAP);
            }
            else if (static_cast<uint8_t>(MsgPackConstants::MAP16) == T) {
                uint16_t tokens{0};
                in.read(reinterpret_cast<char*>(&tokens), sizeof(uint16_t));
                tokensToRead = be16toh(tokens);
            }
            else if (static_cast<uint8_t>(MsgPackConstants::MAP32) == T) {
                in.read(reinterpret_cast<char*>(&tokensToRead), sizeof(uint32_t));
                tokensToRead = be32toh(tokensToRead);
            }

            // Next, read pairs string/value.
std::cout << "Reading " << tokensToRead << std::endl;
            while (0 < tokensToRead) {
                MsgPackKeyValue entry;
                entry.m_key = readString(in);
std::cout << "K = " << entry.m_key << std::endl;

                // Read next byte and determine format family.
                c = static_cast<uint8_t>(in.get());
                entry.m_formatFamily = getFormatFamily(c);

                if (MsgPackConstants::BOOL_FORMAT == entry.m_formatFamily) {
                    entry.m_value = false;
                    if (static_cast<uint8_t>(c) == static_cast<uint8_t>(MsgPackConstants::IS_TRUE)) {
                        entry.m_value = true;
                    }
                    else if (static_cast<uint8_t>(c) == static_cast<uint8_t>(MsgPackConstants::IS_FALSE)) {
                        entry.m_value = false;
                    }
                }
                else if (MsgPackConstants::STR_FORMAT == entry.m_formatFamily) {
                    in.unget(); // Last read character needs to be put back.
                    entry.m_value = readString(in);
                }

                keyValues[entry.m_key] = entry;
                tokensToRead--;
            }
        }
    }
    return keyValues;
}

void FromMsgPackVisitor::decodeFrom(std::istream &in) noexcept {
    (void)in;

    m_keyValues = readKeyValues(in);
}

void FromMsgPackVisitor::preVisit(uint32_t id,
                                       const std::string &shortName,
                                       const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void FromMsgPackVisitor::postVisit() noexcept {}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = linb::any_cast<bool>(m_keyValues[name].m_value);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = linb::any_cast<std::string>(m_keyValues[name].m_value).at(0);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;
    
    (void)name;
    (void)v;
}

} // namespace cluon
