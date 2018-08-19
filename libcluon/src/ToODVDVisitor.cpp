/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cluon/ToODVDVisitor.hpp"

#include <sstream>

namespace cluon {

std::string ToODVDVisitor::messageSpecification() const noexcept {
    std::stringstream tmp;
    for (const auto &e : m_forwardDeclarations) { tmp << e; }
    tmp << m_buffer.str();

    const std::string retVal{tmp.str()};
    return retVal;
}

void ToODVDVisitor::preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)shortName;
    m_buffer << "message " << longName << " [ id = " << id << " ] {" << '\n';
}

void ToODVDVisitor::postVisit() noexcept {
    m_buffer << '}' << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "bool"
             << " " << name << " [ default = false, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "char"
             << " " << name << " [ default = '0', id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "int8"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "uint8"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "int16"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "uint16"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "int32"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "uint32"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "int64"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "uint64"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "float"
             << " " << name << " [ default = 0.0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "double"
             << " " << name << " [ default = 0.0, id = " << id << " ];" << '\n';
}

void ToODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "string"
             << " " << name << " [ default = \"\", id = " << id << " ];" << '\n';
}

} // namespace cluon
