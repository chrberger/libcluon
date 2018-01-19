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

// clang-format off
#ifdef WIN32
    #include <Winsock2.h> // for ntohl, ntohs
#endif
// clang-format on

#include "cluon/FromMsgPackVisitor.hpp"

#include <cstring>
#include <vector>

namespace cluon {

FromMsgPackVisitor::FromMsgPackVisitor() noexcept
    : m_keyValues{m_data} {}

FromMsgPackVisitor::FromMsgPackVisitor(std::map<std::string, FromMsgPackVisitor::MsgPackKeyValue> &preset) noexcept
    : m_keyValues{preset} {}

MsgPackConstants FromMsgPackVisitor::getFormatFamily(uint8_t T) noexcept {
    MsgPackConstants formatFamily{MsgPackConstants::UNKNOWN_FORMAT};

    if (static_cast<uint8_t>(MsgPackConstants::IS_FALSE) == T) {
        formatFamily = MsgPackConstants::BOOL_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::IS_TRUE) == T) {
        formatFamily = MsgPackConstants::BOOL_FORMAT;
    }
    else if (0x7F >= T) {
        formatFamily = MsgPackConstants::UINT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::UINT8) == T) {
        formatFamily = MsgPackConstants::UINT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::UINT16) == T) {
        formatFamily = MsgPackConstants::UINT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::UINT32) == T) {
        formatFamily = MsgPackConstants::UINT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::UINT64) == T) {
        formatFamily = MsgPackConstants::UINT_FORMAT;
    }
    else if ((0xE0 <= T) && (0xFF >= T)) {
        formatFamily = MsgPackConstants::INT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::INT8) == T) {
        formatFamily = MsgPackConstants::INT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::INT16) == T) {
        formatFamily = MsgPackConstants::INT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::INT32) == T) {
        formatFamily = MsgPackConstants::INT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::INT64) == T) {
        formatFamily = MsgPackConstants::INT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::FLOAT) == T) {
        formatFamily = MsgPackConstants::FLOAT_FORMAT;
    }
    else if (static_cast<uint8_t>(MsgPackConstants::DOUBLE) == T) {
        formatFamily = MsgPackConstants::FLOAT_FORMAT;
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

uint64_t FromMsgPackVisitor::readUint(std::istream &in) noexcept {
    uint64_t retVal{0};
    if (in.good()) {
        uint8_t c = static_cast<uint8_t>(in.get());
        if (MsgPackConstants::UINT_FORMAT == getFormatFamily(c)) {
            if (0x7F >= c) {
                retVal = static_cast<uint64_t>(c);
            }
            else if (static_cast<uint8_t>(MsgPackConstants::UINT8) == c) {
                uint8_t v{0};
                in.read(reinterpret_cast<char*>(&v), sizeof(uint8_t));
                retVal = static_cast<uint64_t>(v);
            }
            else if (static_cast<uint8_t>(MsgPackConstants::UINT16) == c) {
                uint16_t v{0};
                in.read(reinterpret_cast<char*>(&v), sizeof(uint16_t));
                v = be16toh(v);
                retVal = static_cast<uint64_t>(v);
            }
            else if (static_cast<uint8_t>(MsgPackConstants::UINT32) == c) {
                uint32_t v{0};
                in.read(reinterpret_cast<char*>(&v), sizeof(uint32_t));
                v = be32toh(v);
                retVal = static_cast<uint64_t>(v);
            }
            else if (static_cast<uint8_t>(MsgPackConstants::UINT64) == c) {
                in.read(reinterpret_cast<char*>(&retVal), sizeof(uint64_t));
                retVal = be64toh(retVal);
            }
        }
    }
    return retVal;
}

int64_t FromMsgPackVisitor::readInt(std::istream &in) noexcept {
    int64_t retVal{0};
    if (in.good()) {
        int8_t c = static_cast<int8_t>(in.get());
        if (MsgPackConstants::INT_FORMAT == getFormatFamily(static_cast<uint8_t>(c))) {
            if ((0xE0 <= static_cast<uint8_t>(c)) && (0xFF >= static_cast<uint8_t>(c))) {
                retVal = static_cast<int64_t>(c);
            }
            else if (static_cast<int8_t>(MsgPackConstants::INT8) == c) {
                int8_t v{0};
                in.read(reinterpret_cast<char*>(&v), sizeof(int8_t));
                retVal = static_cast<int64_t>(v);
            }
            else if (static_cast<int8_t>(MsgPackConstants::INT16) == c) {
                int16_t v{0};
                in.read(reinterpret_cast<char*>(&v), sizeof(int16_t));
                v = static_cast<int16_t>(be16toh(v));
                retVal = static_cast<int64_t>(v);
            }
            else if (static_cast<int8_t>(MsgPackConstants::INT32) == c) {
                int32_t v{0};
                in.read(reinterpret_cast<char*>(&v), sizeof(int32_t));
                v = static_cast<int32_t>(be32toh(v));
                retVal = static_cast<int64_t>(v);
            }
            else if (static_cast<int8_t>(MsgPackConstants::INT64) == c) {
                in.read(reinterpret_cast<char*>(&retVal), sizeof(int64_t));
                retVal = static_cast<int64_t>(be64toh(retVal));
            }
        }
    }
    return retVal;
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
#ifdef WIN32
                for (uint32_t i = 0; i < static_cast<uint32_t>(length); i++) {
                    char c;
                    in.get(c);
                    buffer.push_back(c);
                }
#else
                in.read(static_cast<char *>(&buffer[0]), static_cast<std::streamsize>(length));
#endif
                retVal = std::string(buffer.begin(), buffer.begin() + length);
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
            while (0 < tokensToRead) {
                MsgPackKeyValue entry;
                entry.m_key = readString(in);

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
                else if (MsgPackConstants::UINT_FORMAT == entry.m_formatFamily) {
                    in.unget(); // Last character needs to be put back to process the uints correctly as it might contain the value.
                    entry.m_value = readUint(in);
                }
                else if (MsgPackConstants::INT_FORMAT == entry.m_formatFamily) {
                    in.unget(); // Last character needs to be put back to process the ints correctly as it might contain the value.
                    entry.m_value = readInt(in);
                }
                else if (MsgPackConstants::FLOAT_FORMAT == entry.m_formatFamily) {
                    if (static_cast<uint8_t>(c) == static_cast<uint8_t>(MsgPackConstants::FLOAT)) {
                        uint32_t _v{0};
                        in.read(reinterpret_cast<char*>(&_v), sizeof(uint32_t));
                        _v = be32toh(_v);
                        float v{0.0f};
                        std::memmove(&v, &_v, sizeof(float));
                        entry.m_value = v;
                    }
                    if (static_cast<uint8_t>(c) == static_cast<uint8_t>(MsgPackConstants::DOUBLE)) {
                        uint64_t _v{0};
                        in.read(reinterpret_cast<char*>(&_v), sizeof(uint64_t));
                        _v = be64toh(_v);
                        double v{0.0};
                        std::memmove(&v, &_v, sizeof(double));
                        entry.m_value = v;
                    }
                }
                else if (MsgPackConstants::STR_FORMAT == entry.m_formatFamily) {
                    in.unget(); // Last character needs to be put back to process the string correctly as it might encode its length.
                    entry.m_value = readString(in);
                }
                else if (MsgPackConstants::MAP_FORMAT == entry.m_formatFamily) {
                    in.unget(); // Last character needs to be put back to process the contained nested map correctly as it might encode its length.
                    entry.m_value = readKeyValues(in);
                }

                keyValues[entry.m_key] = entry;
                tokensToRead--;
            }
            // Stop processing further tokens (might be handled from outer decoder).
            break;
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
    if (0 < m_keyValues.count(name)) {
        try {
            v = static_cast<int8_t>(linb::any_cast<int64_t>(m_keyValues[name].m_value));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = static_cast<uint8_t>(linb::any_cast<uint64_t>(m_keyValues[name].m_value));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = static_cast<int16_t>(linb::any_cast<int64_t>(m_keyValues[name].m_value));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = static_cast<uint16_t>(linb::any_cast<uint64_t>(m_keyValues[name].m_value));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = static_cast<int32_t>(linb::any_cast<int64_t>(m_keyValues[name].m_value));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = static_cast<uint32_t>(linb::any_cast<uint64_t>(m_keyValues[name].m_value));
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = linb::any_cast<int64_t>(m_keyValues[name].m_value);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = linb::any_cast<uint64_t>(m_keyValues[name].m_value);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = linb::any_cast<float>(m_keyValues[name].m_value);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = linb::any_cast<double>(m_keyValues[name].m_value);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void FromMsgPackVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)id;
    (void)typeName;
    if (0 < m_keyValues.count(name)) {
        try {
            v = linb::any_cast<std::string>(m_keyValues[name].m_value);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

} // namespace cluon
