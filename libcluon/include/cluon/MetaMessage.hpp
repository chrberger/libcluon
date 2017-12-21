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

#ifndef METAMESSAGE_HPP
#define METAMESSAGE_HPP

#include "cluon/cluon.hpp"

#include <functional>
#include <string>
#include <vector>

namespace cluon {
/**
This class provides a generic description for a message. It is internally
used during the processing of message specifications to generate targets
like C++ or .proto files.
*/
class LIBCLUON_API MetaMessage {
   public:
    /**
     * This inner class provides a generic description for a message's fields.
     * It is internally used during the processing of message specifications.
     */
    class MetaField {
       public:
        enum MetaFieldDataTypes : uint16_t {
            BOOL_T      = 0,
            UINT8_T     = 2,
            INT8_T      = 3,
            UINT16_T    = 4,
            INT16_T     = 5,
            UINT32_T    = 6,
            INT32_T     = 7,
            UINT64_T    = 8,
            INT64_T     = 9,
            CHAR_T      = 11,
            FLOAT_T     = 13,
            DOUBLE_T    = 14,
            BYTES_T     = 49,
            STRING_T    = 51,
            MESSAGE_T   = 53,
            UNDEFINED_T = 0xFFFF,
        };

       private:
        MetaField &operator=(MetaField &&) = delete;

       public:
        MetaField()                  = default;
        MetaField(const MetaField &) = default;
        MetaField(MetaField &&)      = default;
        MetaField &operator=(const MetaField &) = default;

       public:
        /**
         * @return Type of this field.
         */
        MetaFieldDataTypes fieldDataType() const noexcept;
        /**
         * This method sets the type for this field.
         *
         * @param v Type for this field.
         * @return Reference to this instance.
         */
        MetaField &fieldDataType(const MetaFieldDataTypes &v) noexcept;

        /**
         * @return Type name of this field.
         */
        std::string fieldDataTypeName() const noexcept;
        /**
         * This method sets the type name for this field.
         *
         * @param v Type name for this field.
         * @return Reference to this instance.
         */
        MetaField &fieldDataTypeName(const std::string &v) noexcept;

        /**
         * @return Name of this field.
         */
        std::string fieldName() const noexcept;
        /**
         * This method sets the name for this field.
         *
         * @param v Name for this field.
         * @return Reference to this instance.
         */
        MetaField &fieldName(const std::string &v) noexcept;

        /**
         * @return Identifier of this field.
         */
        uint32_t fieldIdentifier() const noexcept;
        /**
         * This method sets the identifier for this field.
         *
         * @param v Identifier for this field.
         * @return Reference to this instance.
         */
        MetaField &fieldIdentifier(uint32_t v) noexcept;

        /**
         * @return Field's default initialization value.
         */
        std::string defaultInitializationValue() const noexcept;
        /**
         * This method sets the field's default initialization value for this field.
         *
         * @param v Field's default initialization value for this field.
         * @return Reference to this instance.
         */
        MetaField &defaultInitializationValue(const std::string &v) noexcept;

       private:
        MetaFieldDataTypes m_fieldDataType{UNDEFINED_T};
        std::string m_fieldDataTypeName{""};
        std::string m_fieldName{""};
        uint32_t m_fieldIdentifier{0};
        std::string m_defaultInitializationValue{""};
    };

   public:
    MetaMessage()                    = default;
    MetaMessage(const MetaMessage &) = default;
    MetaMessage(MetaMessage &&)      = default;
    MetaMessage &operator=(const MetaMessage &) = default;
    MetaMessage &operator=(MetaMessage &&) = default;

    /**
     * This method adds a metafield to this meta message.
     *
     * @param mf Meta field to be added.
     * @return Reference to this instance.
     */
    MetaMessage &add(MetaField &&mf) noexcept;

    /**
     * This method returns a vector of current meta fields.
     *
     * @return Meta fields from this meta message.
     */
    const std::vector<MetaField> &listOfMetaFields() const noexcept;

    /**
     * This method can be used to visit this instance and propagate information
     * details about the contained fields.
     *
     * @param visit std::function object to be called to visit this MetaMessage.
     */
    void accept(const std::function<void(const MetaMessage &)> &visit);

    /**
     * @return Package name.
     */
    std::string packageName() const noexcept;
    /**
     * This method sets the package name.
     *
     * @param v Package name for this message.
     * @return Reference to this instance.
     */
    MetaMessage &packageName(const std::string &v) noexcept;

    /**
     * @return Message name.
     */
    std::string messageName() const noexcept;
    /**
     * This method sets the message name.
     *
     * @param v Message name for this message.
     * @return Reference to this instance.
     */
    MetaMessage &messageName(const std::string &v) noexcept;

    /**
     * @return Message identifier.
     */
    uint32_t messageIdentifier() const noexcept;
    /**
     * This method sets the message identifier.
     *
     * @param v Message identifier for this message.
     * @return Reference to this instance.
     */
    MetaMessage &messageIdentifier(uint32_t v) noexcept;

   private:
    std::string m_packageName{""};
    std::string m_messageName{""};
    uint32_t m_messageIdentifier{0};
    std::vector<MetaField> m_listOfMetaFields{};
};
} // namespace cluon

#endif
