/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_TOLCMVISITOR_HPP
#define CLUON_TOLCMVISITOR_HPP

#include "cluon/cluon.hpp"

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace cluon {
/**
This class encodes a given message in LCM format.
*/
class LIBCLUON_API ToLCMVisitor {
   private:
    ToLCMVisitor(const ToLCMVisitor &) = delete;
    ToLCMVisitor(ToLCMVisitor &&)      = delete;
    ToLCMVisitor &operator=(const ToLCMVisitor &) = delete;
    ToLCMVisitor &operator=(ToLCMVisitor &&) = delete;

   public:
    ToLCMVisitor()  = default;
    ~ToLCMVisitor() = default;

    /**
     * @param withHash True if the hash value from the fields shall be included.
     * @return Encoded data in LCM format.
     */
    std::string encodedData(bool withHash = true) const noexcept;

   public:
    // The following methods are provided to allow an instance of this class to
    // be used as visitor for an instance with the method signature void accept<T>(T&);

    void preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept;
    void postVisit() noexcept;

    void visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept;

    template <typename T>
    void visit(uint32_t &id, std::string &&typeName, std::string &&name, T &value) noexcept {
        (void)id;
        (void)typeName;
        calculateHash(name);
        calculateHash(0);

        // No hash for the type but for name and dimension.
        cluon::ToLCMVisitor nestedLCMEncoder;
        value.accept(nestedLCMEncoder);

        constexpr bool WITH_HASH{false};
        const std::string s = nestedLCMEncoder.encodedData(WITH_HASH);
        m_buffer.write(s.c_str(), static_cast<std::streamsize>(s.size()));

        // Save this complex field's hash for later to compute final hash.
        m_hashes.push_back(nestedLCMEncoder.hash());
    }

   private:
    int64_t hash() const noexcept;
    void calculateHash(char c) noexcept;
    void calculateHash(const std::string &s) noexcept;

   private:
    int64_t m_hash{0x12345678};
    std::stringstream m_buffer{""};
    std::vector<int64_t> m_hashes{};
};
} // namespace cluon

#endif
