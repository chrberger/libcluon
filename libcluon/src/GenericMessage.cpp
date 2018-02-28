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

#include "cluon/GenericMessage.hpp"

#include <istream>
#include <iterator>
#include <regex>

namespace cluon {

void GenericMessage::GenericMessageVisitor::preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)longName;
    m_metaMessage.messageIdentifier(id).messageName(shortName);
    if (!longName.empty()) {
        const auto pos = longName.rfind(shortName);
        if (std::string::npos != pos) {
            m_metaMessage.packageName(longName.substr(0, pos - 1));
        }
    }
}

void GenericMessage::GenericMessageVisitor::postVisit() noexcept {}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::BOOL_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::CHAR_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::INT8_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::UINT8_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::INT16_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::UINT16_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::INT32_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::UINT32_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::INT64_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::UINT64_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::FLOAT_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::DOUBLE_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id).fieldDataType(cluon::MetaMessage::MetaField::STRING_T).fieldDataTypeName(typeName).fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

MetaMessage GenericMessage::GenericMessageVisitor::metaMessage() const noexcept {
    return m_metaMessage;
}

std::map<uint32_t, linb::any> GenericMessage::GenericMessageVisitor::intermediateDataRepresentation() const noexcept {
    return m_intermediateDataRepresentation;
}

////////////////////////////////////////////////////////////////////////////////

int32_t GenericMessage::ID() {
    return m_metaMessage.messageIdentifier();
}

const std::string GenericMessage::ShortName() {
    std::string tmp{LongName()};
    std::replace(tmp.begin(), tmp.end(), '.', ' ');
    std::istringstream sstr{tmp};
    std::vector<std::string> tokens{std::istream_iterator<std::string>(sstr), std::istream_iterator<std::string>()};

    return tokens.back();
}

const std::string GenericMessage::LongName() {
    return m_metaMessage.packageName() + (!m_metaMessage.packageName().empty() ? "." : "") + m_metaMessage.messageName();
}

void GenericMessage::preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
}

void GenericMessage::postVisit() noexcept {}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<bool>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<char>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<int8_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<uint8_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<int16_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<uint16_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<int32_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<uint32_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<int64_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<uint64_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<float>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<double>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<std::string>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void GenericMessage::createFrom(const MetaMessage &mm, const std::vector<MetaMessage> &mms) noexcept {
    m_metaMessage = mm;
    m_longName    = m_metaMessage.messageName();

    m_scopeOfMetaMessages.clear();
    m_scopeOfMetaMessages = mms;

    m_mapForScopeOfMetaMessages.clear();
    for (const auto &e : m_scopeOfMetaMessages) { m_mapForScopeOfMetaMessages[e.messageName()] = e; }

    m_intermediateDataRepresentation.clear();
    for (const auto &f : m_metaMessage.listOfMetaFields()) {
        if (f.fieldDataType() == MetaMessage::MetaField::BOOL_T) {
            try {
                linb::any _v{false};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::CHAR_T) {
            try {
                linb::any _v{static_cast<char>('\0')};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::UINT8_T) {
            try {
                linb::any _v{static_cast<uint8_t>(0)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::INT8_T) {
            try {
                linb::any _v{static_cast<int8_t>(0)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::UINT16_T) {
            try {
                linb::any _v{static_cast<uint16_t>(0)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::INT16_T) {
            try {
                linb::any _v{static_cast<int16_t>(0)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::UINT32_T) {
            try {
                linb::any _v{static_cast<uint32_t>(0)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::INT32_T) {
            try {
                linb::any _v{static_cast<int32_t>(0)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::UINT64_T) {
            try {
                linb::any _v{static_cast<uint64_t>(0)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::INT64_T) {
            try {
                linb::any _v{static_cast<int64_t>(0)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::FLOAT_T) {
            try {
                linb::any _v{static_cast<float>(0.0f)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::DOUBLE_T) {
            try {
                linb::any _v{static_cast<double>(0.0)};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if ((f.fieldDataType() == MetaMessage::MetaField::STRING_T) || (f.fieldDataType() == MetaMessage::MetaField::BYTES_T)) {
            try {
                linb::any _v                                          = std::string{};
                m_intermediateDataRepresentation[f.fieldIdentifier()] = _v;
            } catch (const linb::bad_any_cast &) { // LCOV_EXCL_LINE
            }
        } else if (f.fieldDataType() == MetaMessage::MetaField::MESSAGE_T) {
            if (0 < m_mapForScopeOfMetaMessages.count(f.fieldDataTypeName())) {
                // Create a GenericMessage from the decoded Proto-data.
                cluon::GenericMessage gm;
                gm.createFrom(m_mapForScopeOfMetaMessages[f.fieldDataTypeName()], m_scopeOfMetaMessages);

                m_intermediateDataRepresentation[f.fieldIdentifier()] = linb::any{gm};
            }
        }
    }
}

} // namespace cluon
