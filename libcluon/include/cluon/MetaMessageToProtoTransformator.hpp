/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_METAMESSAGETOPROTOTRANSFORMATOR_HPP
#define CLUON_METAMESSAGETOPROTOTRANSFORMATOR_HPP

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
