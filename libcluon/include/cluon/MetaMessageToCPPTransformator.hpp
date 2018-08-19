/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_METAMESSAGETOCPPTRANSFORMATOR_HPP
#define CLUON_METAMESSAGETOCPPTRANSFORMATOR_HPP

#include "Mustache/mustache.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/cluon.hpp"

#include <string>

namespace cluon {
/**
This class transforms a given MetaMessage to a C++ header and source content.

Transformation is using https://github.com/kainjow/Mustache.
*/
class LIBCLUON_API MetaMessageToCPPTransformator {
   private:
    MetaMessageToCPPTransformator(MetaMessageToCPPTransformator &&) = delete;
    MetaMessageToCPPTransformator &operator=(const MetaMessageToCPPTransformator &) = delete;
    MetaMessageToCPPTransformator &operator=(MetaMessageToCPPTransformator &&) = delete;

   public:
    MetaMessageToCPPTransformator()                                      = default;
    MetaMessageToCPPTransformator(const MetaMessageToCPPTransformator &) = default;

    /**
     * The method is called from MetaMessage to visit itself using this transformator.
     *
     * @param mm MetaMessage to visit.
     */
    void visit(const MetaMessage &mm) noexcept;

    /**
     * @return Content of the C++ header.
     */
    std::string content() noexcept;

   private:
    kainjow::mustache::data m_dataToBeRendered{};
    kainjow::mustache::data m_fields{kainjow::mustache::data::type::list};
};
} // namespace cluon

#endif
