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

#include "cluon/MetaMessage.hpp"

namespace cluon {
MetaMessage::MetaField::MetaFieldDataTypes MetaMessage::MetaField::fieldDataType() const noexcept {
    return m_fieldDataType;
}

MetaMessage::MetaField &
MetaMessage::MetaField::fieldDataType(const MetaMessage::MetaField::MetaFieldDataTypes &v) noexcept {
    m_fieldDataType = v;
    return *this;
}

std::string MetaMessage::MetaField::fieldDataTypeName() const noexcept {
    return m_fieldDataTypeName;
}

MetaMessage::MetaField &MetaMessage::MetaField::fieldDataTypeName(const std::string &v) noexcept {
    m_fieldDataTypeName = v;
    return *this;
}

std::string MetaMessage::MetaField::fieldName() const noexcept {
    return m_fieldName;
}

MetaMessage::MetaField &MetaMessage::MetaField::fieldName(const std::string &v) noexcept {
    m_fieldName = v;
    return *this;
}

uint32_t MetaMessage::MetaField::fieldIdentifier() const noexcept {
    return m_fieldIdentifier;
}

MetaMessage::MetaField &MetaMessage::MetaField::fieldIdentifier(uint32_t v) noexcept {
    m_fieldIdentifier = v;
    return *this;
}

std::string MetaMessage::MetaField::defaultInitializationValue() const noexcept {
    return m_defaultInitializationValue;
}

MetaMessage::MetaField &MetaMessage::MetaField::defaultInitializationValue(const std::string &v) noexcept {
    m_defaultInitializationValue = v;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

std::string MetaMessage::packageName() const noexcept {
    return m_packageName;
}

std::string MetaMessage::messageName() const noexcept {
    return m_messageName;
}

MetaMessage &MetaMessage::packageName(const std::string &v) noexcept {
    m_packageName = v;
    return *this;
}

MetaMessage &MetaMessage::messageName(const std::string &v) noexcept {
    m_messageName = v;
    return *this;
}

uint32_t MetaMessage::messageIdentifier() const noexcept {
    return m_messageIdentifier;
}

MetaMessage &MetaMessage::messageIdentifier(uint32_t v) noexcept {
    m_messageIdentifier = v;
    return *this;
}

MetaMessage &MetaMessage::add(MetaMessage::MetaField &&mf) noexcept {
    m_listOfMetaFields.emplace_back(std::move(mf));
    return *this;
}

const std::vector<MetaMessage::MetaField> &MetaMessage::listOfMetaFields() const noexcept {
    return m_listOfMetaFields;
}

void MetaMessage::accept(const std::function<void(const MetaMessage &)> &visit) {
    visit(*this);
}
} // namespace cluon
