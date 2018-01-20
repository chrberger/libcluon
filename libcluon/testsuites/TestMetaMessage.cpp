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

#include "catch.hpp"

#include "cluon/MetaMessage.hpp"

TEST_CASE("Empty MetaField.") {
    cluon::MetaMessage mm;
    REQUIRE(mm.packageName().empty());
    REQUIRE(mm.messageName().empty());
    REQUIRE(0 == mm.messageIdentifier());
    REQUIRE(mm.listOfMetaFields().empty());

    cluon::MetaMessage::MetaField mf;
    REQUIRE(cluon::MetaMessage::MetaField::UNDEFINED_T == mf.fieldDataType());
    REQUIRE(mf.fieldDataTypeName().empty());
    REQUIRE(mf.fieldName().empty());
    REQUIRE(0 == mf.fieldIdentifier());
    REQUIRE(mf.defaultInitializationValue().empty());
}

TEST_CASE("MetaField with some values.") {
    cluon::MetaMessage mm;
    REQUIRE(mm.packageName().empty());
    REQUIRE(mm.messageName().empty());
    REQUIRE(0 == mm.messageIdentifier());
    REQUIRE(mm.listOfMetaFields().empty());

    cluon::MetaMessage::MetaField mf;
    REQUIRE(cluon::MetaMessage::MetaField::UNDEFINED_T == mf.fieldDataType());
    REQUIRE(mf.fieldDataTypeName().empty());
    REQUIRE(mf.fieldName().empty());
    REQUIRE(0 == mf.fieldIdentifier());
    REQUIRE(mf.defaultInitializationValue().empty());

    mm.packageName("Hello");
    mm.messageName("World");
    mm.messageIdentifier(1234);
    REQUIRE("Hello" == mm.packageName());
    REQUIRE("World" == mm.messageName());
    REQUIRE(1234 == mm.messageIdentifier());

    mf.fieldDataType(cluon::MetaMessage::MetaField::BOOL_T);
    mf.fieldDataTypeName("bool");
    mf.fieldName("Citizens");
    mf.fieldIdentifier(123);
    mf.defaultInitializationValue("XYZ");
    REQUIRE(cluon::MetaMessage::MetaField::BOOL_T == mf.fieldDataType());
    REQUIRE("bool" == mf.fieldDataTypeName());
    REQUIRE("Citizens" == mf.fieldName());
    REQUIRE(123 == mf.fieldIdentifier());
    REQUIRE("XYZ" == mf.defaultInitializationValue());

    {
        cluon::MetaMessage::MetaField mf2;

        REQUIRE(cluon::MetaMessage::MetaField::UNDEFINED_T == mf2.fieldDataType());
        REQUIRE(mf2.fieldDataTypeName().empty());
        REQUIRE(mf2.fieldName().empty());
        REQUIRE(0 == mf2.fieldIdentifier());
        REQUIRE(mf2.defaultInitializationValue().empty());

        mf2 = mf;

        REQUIRE(cluon::MetaMessage::MetaField::BOOL_T == mf2.fieldDataType());
        REQUIRE("bool" == mf2.fieldDataTypeName());
        REQUIRE("Citizens" == mf2.fieldName());
        REQUIRE(123 == mf2.fieldIdentifier());
        REQUIRE("XYZ" == mf2.defaultInitializationValue());
    }
}

TEST_CASE("Empty MetaMessage.") {
    cluon::MetaMessage mm;
    REQUIRE(mm.packageName().empty());
    REQUIRE(mm.messageName().empty());
    REQUIRE(0 == mm.messageIdentifier());
    REQUIRE(mm.listOfMetaFields().empty());

    cluon::MetaMessage::MetaField mf;
    REQUIRE(cluon::MetaMessage::MetaField::UNDEFINED_T == mf.fieldDataType());
    REQUIRE(mf.fieldDataTypeName().empty());
    REQUIRE(mf.fieldName().empty());
    REQUIRE(0 == mf.fieldIdentifier());
    REQUIRE(mf.defaultInitializationValue().empty());

    mm.add(std::move(mf));

    auto tmp = mm.listOfMetaFields();
    REQUIRE(!tmp.empty());
    REQUIRE(cluon::MetaMessage::MetaField::UNDEFINED_T == tmp.front().fieldDataType());
    REQUIRE(mm.packageName().empty());
    REQUIRE(mm.messageName().empty());
    REQUIRE(0 == mm.messageIdentifier());
    REQUIRE(tmp.front().fieldName().empty());
    REQUIRE(tmp.front().fieldDataTypeName().empty());
    REQUIRE(0 == tmp.front().fieldIdentifier());
    REQUIRE(tmp.front().defaultInitializationValue().empty());
}

TEST_CASE("MetaMessage with some values.") {
    cluon::MetaMessage mm;
    REQUIRE(mm.packageName().empty());
    REQUIRE(mm.messageName().empty());
    REQUIRE(0 == mm.messageIdentifier());
    REQUIRE(mm.listOfMetaFields().empty());

    cluon::MetaMessage::MetaField mf;
    REQUIRE(cluon::MetaMessage::MetaField::UNDEFINED_T == mf.fieldDataType());
    REQUIRE(mf.fieldDataTypeName().empty());
    REQUIRE(mf.fieldName().empty());
    REQUIRE(0 == mf.fieldIdentifier());
    REQUIRE(mf.defaultInitializationValue().empty());

    mm.packageName("Hello");
    mm.messageName("World");
    mm.messageIdentifier(1234);
    REQUIRE("Hello" == mm.packageName());
    REQUIRE("World" == mm.messageName());
    REQUIRE(1234 == mm.messageIdentifier());

    mf.fieldDataType(cluon::MetaMessage::MetaField::BOOL_T);
    mf.fieldDataTypeName("bool");
    mf.fieldName("Citizens");
    mf.fieldIdentifier(123);
    mf.defaultInitializationValue("XYZ");
    REQUIRE(cluon::MetaMessage::MetaField::BOOL_T == mf.fieldDataType());
    REQUIRE("bool" == mf.fieldDataTypeName());
    REQUIRE("Citizens" == mf.fieldName());
    REQUIRE(123 == mf.fieldIdentifier());
    REQUIRE("XYZ" == mf.defaultInitializationValue());

    mm.add(std::move(mf));

    auto tmp = mm.listOfMetaFields();
    REQUIRE(!tmp.empty());
    REQUIRE("Hello" == mm.packageName());
    REQUIRE("World" == mm.messageName());
    REQUIRE(1234 == mm.messageIdentifier());
    REQUIRE(cluon::MetaMessage::MetaField::BOOL_T == tmp.front().fieldDataType());
    REQUIRE("bool" == tmp.front().fieldDataTypeName());
    REQUIRE("Citizens" == tmp.front().fieldName());
    REQUIRE(123 == tmp.front().fieldIdentifier());
    REQUIRE("XYZ" == tmp.front().defaultInitializationValue());
}

TEST_CASE("MetaMessage with visitor.") {
    cluon::MetaMessage mm;
    mm.packageName("Hello");
    mm.messageName("World");
    mm.messageIdentifier(1234);
    REQUIRE(mm.listOfMetaFields().empty());

    cluon::MetaMessage::MetaField mf;
    mf.fieldDataType(cluon::MetaMessage::MetaField::BOOL_T);
    mf.fieldDataTypeName("bool");
    mf.fieldName("Citizens");
    mf.fieldIdentifier(123);
    mf.defaultInitializationValue("XYZ");
    mm.add(std::move(mf));

    auto tmp = mm.listOfMetaFields();
    REQUIRE("Hello" == mm.packageName());
    REQUIRE("World" == mm.messageName());
    REQUIRE(1234 == mm.messageIdentifier());
    REQUIRE(!tmp.empty());
    REQUIRE(cluon::MetaMessage::MetaField::BOOL_T == tmp.front().fieldDataType());
    REQUIRE("bool" == tmp.front().fieldDataTypeName());
    REQUIRE("Citizens" == tmp.front().fieldName());
    REQUIRE(123 == tmp.front().fieldIdentifier());
    REQUIRE("XYZ" == tmp.front().defaultInitializationValue());

    class InternalVisitor {
       public:
        cluon::MetaMessage m_mm{};

        void visit(const cluon::MetaMessage &_mm) noexcept { m_mm = _mm; }
    };
    InternalVisitor iv;

    REQUIRE(iv.m_mm.packageName().empty());
    REQUIRE(iv.m_mm.messageName().empty());
    REQUIRE(0 == iv.m_mm.messageIdentifier());
    REQUIRE(iv.m_mm.listOfMetaFields().empty());

    mm.accept([&_iv = iv](const cluon::MetaMessage &_mm) { _iv.visit(_mm); });

    REQUIRE("Hello" == iv.m_mm.packageName());
    REQUIRE("World" == iv.m_mm.messageName());
    REQUIRE(1234 == iv.m_mm.messageIdentifier());
    REQUIRE(!iv.m_mm.listOfMetaFields().empty());
    REQUIRE(cluon::MetaMessage::MetaField::BOOL_T == iv.m_mm.listOfMetaFields().front().fieldDataType());
    REQUIRE("bool" == iv.m_mm.listOfMetaFields().front().fieldDataTypeName());
    REQUIRE("Citizens" == iv.m_mm.listOfMetaFields().front().fieldName());
    REQUIRE(123 == iv.m_mm.listOfMetaFields().front().fieldIdentifier());
    REQUIRE("XYZ" == iv.m_mm.listOfMetaFields().front().defaultInitializationValue());
}
