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

#include "cluon/ODVDVisitor.hpp"

#include <sstream>

namespace cluon {

std::string ODVDVisitor::messageSpecification() const noexcept {
    std::stringstream tmp;
    for (const auto &e : m_forwardDeclarations) { tmp << e; }
    tmp << m_buffer.str();

    const std::string retVal{tmp.str()};
    return retVal;
}

void ODVDVisitor::preVisit(uint32_t id, const std::string &shortName, const std::string &longName) noexcept {
    (void)shortName;
    m_buffer << "message " << longName << " [ id = " << id << " ] {" << '\n';
}

void ODVDVisitor::postVisit() noexcept {
    m_buffer << "}" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "bool"
             << " " << name << " [ default = false, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "char"
             << " " << name << " [ default = '0', id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "int8"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "uint8"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "int16"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "uint16"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "int32"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "uint32"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "int64"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "uint64"
             << " " << name << " [ default = 0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "float"
             << " " << name << " [ default = 0.0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "double"
             << " " << name << " [ default = 0.0, id = " << id << " ];" << '\n';
}

void ODVDVisitor::visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept {
    (void)typeName;
    (void)v;
    m_buffer << "    "
             << "string"
             << " " << name << " [ default = \"\", id = " << id << " ];" << '\n';
}

} // namespace cluon
