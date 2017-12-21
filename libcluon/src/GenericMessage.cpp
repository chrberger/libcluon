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

#include "cluon/GenericMessage.hpp"

namespace cluon {

void GenericMessage::GenericMessageVisitor::preVisit(uint32_t id,
                                                     const std::string &shortName,
                                                     const std::string &longName) noexcept {
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

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  bool &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::BOOL_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  char &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::CHAR_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  int8_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::INT8_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  uint8_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::UINT8_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  int16_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::INT16_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  uint16_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::UINT16_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  int32_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::INT32_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  uint32_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::UINT32_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  int64_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::INT64_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  uint64_t &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::UINT64_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  float &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::FLOAT_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  double &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::DOUBLE_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
    m_intermediateDataRepresentation[mf.fieldIdentifier()] = linb::any{v};
    m_metaMessage.add(std::move(mf));
}

void GenericMessage::GenericMessageVisitor::visit(uint32_t id,
                                                  std::string &&typeName,
                                                  std::string &&name,
                                                  std::string &v) noexcept {
    cluon::MetaMessage::MetaField mf;
    mf.fieldIdentifier(id)
        .fieldDataType(cluon::MetaMessage::MetaField::STRING_T)
        .fieldDataTypeName(typeName)
        .fieldName(name);
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

void GenericMessage::preVisit(uint32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)id;
    (void)shortName;
    (void)longName;
    // TODO: Set packageName etc from here?
}

void GenericMessage::postVisit() noexcept {}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<bool>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<char>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<int8_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<uint8_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<int16_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<uint16_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<int32_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<uint32_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<int64_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<uint64_t>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<float>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<double>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

void GenericMessage::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    (void)name;
    if (0 < m_intermediateDataRepresentation.count(id)) {
        try {
            v = linb::any_cast<std::string>(m_intermediateDataRepresentation[id]);
        } catch (const linb::bad_any_cast &) {}
    }
}

////////////////////////////////////////////////////////////////////////////////

void GenericMessage::createFrom(const MetaMessage &mm,
                                const std::vector<MetaMessage> &mms,
                                MessageFromProtoDecoder &pd) noexcept {
    m_metaMessage = mm;
    m_longName    = m_metaMessage.messageName();

    m_scopeOfMetaMessages.clear();
    m_scopeOfMetaMessages = mms;

    m_mapForScopeOfMetaMessages.clear();
    for (const auto &e : m_scopeOfMetaMessages) { m_mapForScopeOfMetaMessages[e.messageName()] = e; }

    createIntermediateDataRepresentationFrom(pd);
}

} // namespace cluon
