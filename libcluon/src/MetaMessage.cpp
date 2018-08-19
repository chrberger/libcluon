/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/MetaMessage.hpp"

namespace cluon {

MetaMessage::MetaField::MetaFieldDataTypes MetaMessage::MetaField::fieldDataType() const noexcept {
    return m_fieldDataType;
}

MetaMessage::MetaField &MetaMessage::MetaField::fieldDataType(const MetaMessage::MetaField::MetaFieldDataTypes &v) noexcept {
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

MetaMessage::MetaMessage() noexcept {}

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

int32_t MetaMessage::messageIdentifier() const noexcept {
    return m_messageIdentifier;
}

MetaMessage &MetaMessage::messageIdentifier(int32_t v) noexcept {
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
