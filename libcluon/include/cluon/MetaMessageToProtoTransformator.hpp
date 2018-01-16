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

#ifndef METAMESSAGETOPROTOTRANSFORMATOR_HPP
#define METAMESSAGETOPROTOTRANSFORMATOR_HPP

#include "Mustache/mustache.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/cluon.hpp"

#include <string>

namespace cluon {
/**
This class transforms a given MetaMessage to a .proto file.

Transformation is using https://github.com/kainjow/Mustache.
*/
class LIBCLUON_API MetaMessageToProtoTransformator {
   private:
    MetaMessageToProtoTransformator(MetaMessageToProtoTransformator &&) = delete;
    MetaMessageToProtoTransformator &operator=(const MetaMessageToProtoTransformator &) = delete;
    MetaMessageToProtoTransformator &operator=(MetaMessageToProtoTransformator &&) = delete;

   public:
    MetaMessageToProtoTransformator()                                        = default;
    MetaMessageToProtoTransformator(const MetaMessageToProtoTransformator &) = default;

    /**
     * The method is called from MetaMessage to visit itself using this transformator.
     *
     * @param mm MetaMessage to visit.
     */
    void visit(const MetaMessage &mm) noexcept;

    /**
     * @return Content of the .proto file.
     */
    std::string content(bool withProtoHeader) noexcept;

   private:
    kainjow::mustache::data m_dataToBeRendered;
    kainjow::mustache::data m_fields{kainjow::mustache::data::type::list};
};
} // namespace cluon

#endif
