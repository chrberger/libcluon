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

void GenericMessage::setMetaMessage(const MetaMessage &mm,
                                    const std::vector<MetaMessage> &mms,
                                    MessageFromProtoDecoder &pd) noexcept {
    m_metaMessage = mm;
    m_longName    = m_metaMessage.messageName();
    m_scopeOfMetaMessages.clear();
    m_scopeOfMetaMessages = mms;
    m_mapForScopeOfMetaMessages.clear();
    for (const auto &e : m_scopeOfMetaMessages) { m_mapForScopeOfMetaMessages[e.messageName()] = e; }

    createIntermediateRepresentationFrom(pd);
}

} // namespace cluon
