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

#include "cluon/MessageParser.hpp"
#include "cluon/MetaMessage.hpp"
#include "cluon/MetaMessageToCPPTransformator.hpp"
#include <iostream>
#include <string>

TEST_CASE("Transforming empty message specification.") {
    const char *input = R"(
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(retVal.first.empty());
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
}

TEST_CASE("Transforming broken message specification.") {
    const char *input = R"(
Hello World
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(retVal.first.empty());
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::SYNTAX_ERROR == retVal.second);
}

TEST_CASE("Transforming one message without package and one field.") {
    const char *input = R"(
message MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_HEADER = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYMESSAGE1_HPP
#define MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>

using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};


template<>
struct isVisitable<MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */


int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyMessage1";
}

MyMessage1& MyMessage1::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage1::s() const noexcept {
    return m_s;
}


)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE));
}

TEST_CASE("Transforming two messages without package and one field.") {
    const char *input = R"(
message MyMessage1 [id = 1] {
    string s [id = 1];
}

message MyMessage2 [id = 2] {
    string s [id = 1];
}
)";

    const char *EXPECTED_HEADER1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYMESSAGE1_HPP
#define MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>

using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};


template<>
struct isVisitable<MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */


int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyMessage1";
}

MyMessage1& MyMessage1::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage1::s() const noexcept {
    return m_s;
}


)";

    const char *EXPECTED_HEADER2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYMESSAGE2_HPP
#define MYMESSAGE2_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>

using namespace std::string_literals; // NOLINT
class LIB_API MyMessage2 {
    public:
        MyMessage2() = default;
        MyMessage2(const MyMessage2&) = default;
        MyMessage2& operator=(const MyMessage2&) = default;
        MyMessage2(MyMessage2&&) = default; // NOLINT
        MyMessage2& operator=(MyMessage2&&) = default; // NOLINT
        ~MyMessage2() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage2& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};


template<>
struct isVisitable<MyMessage2> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyMessage2> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */


int32_t MyMessage2::ID() {
    return 2;
}

const std::string MyMessage2::ShortName() {
    return "MyMessage2";
}
const std::string MyMessage2::LongName() {
    return "MyMessage2";
}

MyMessage2& MyMessage2::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage2::s() const noexcept {
    return m_s;
}


)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());
    {
        auto firstMetaMessage = retVal.first.front();
        cluon::MetaMessageToCPPTransformator t;
        firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        //        std::cout << t.contentHeader() << std::endl;
        //        std::cout << t.contentSource() << std::endl;
        REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER1));
        REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE1));
    }
    {
        auto secondMetaMessage = retVal.first.back();
        cluon::MetaMessageToCPPTransformator t;
        secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        //        std::cout << t.contentHeader() << std::endl;
        //        std::cout << t.contentSource() << std::endl;
        REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER2));
        REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE2));
    }
}

TEST_CASE("Transforming two messages with message prefix, no package, and one field.") {
    const char *input = R"(
message MyPackage.MyMessage1 [id = 1] {
    string s [id = 1];
}

message MyPackage.MyMessage2 [id = 2] {
    string s [id = 1];
}
)";

    const char *EXPECTED_HEADER1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE_MYMESSAGE1_HPP
#define MYPACKAGE_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}

template<>
struct isVisitable<MyPackage::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyPackage.MyMessage1";
}

MyMessage1& MyMessage1::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage1::s() const noexcept {
    return m_s;
}

}
)";

    const char *EXPECTED_HEADER2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE_MYMESSAGE2_HPP
#define MYPACKAGE_MYMESSAGE2_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage2 {
    public:
        MyMessage2() = default;
        MyMessage2(const MyMessage2&) = default;
        MyMessage2& operator=(const MyMessage2&) = default;
        MyMessage2(MyMessage2&&) = default; // NOLINT
        MyMessage2& operator=(MyMessage2&&) = default; // NOLINT
        ~MyMessage2() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage2& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}

template<>
struct isVisitable<MyPackage::MyMessage2> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage::MyMessage2> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage {

int32_t MyMessage2::ID() {
    return 2;
}

const std::string MyMessage2::ShortName() {
    return "MyMessage2";
}
const std::string MyMessage2::LongName() {
    return "MyPackage.MyMessage2";
}

MyMessage2& MyMessage2::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage2::s() const noexcept {
    return m_s;
}

}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());
    {
        auto firstMetaMessage = retVal.first.front();
        cluon::MetaMessageToCPPTransformator t;
        firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        //        std::cout << t.contentHeader() << std::endl;
        //        std::cout << t.contentSource() << std::endl;
        REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER1));
        REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE1));
    }
    {
        auto secondMetaMessage = retVal.first.back();
        cluon::MetaMessageToCPPTransformator t;
        secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        //        std::cout << t.contentHeader() << std::endl;
        //        std::cout << t.contentSource() << std::endl;
        REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER2));
        REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE2));
    }
}

TEST_CASE("Transforming two messages with package and one field.") {
    const char *input = R"(
package MyPackage;
message MyMessage1 [id = 1] {
    string s [id = 1];
}

message MyMessage2 [id = 2] {
    string s [id = 1];
}
)";

    const char *EXPECTED_HEADER1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE_MYMESSAGE1_HPP
#define MYPACKAGE_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}

template<>
struct isVisitable<MyPackage::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyPackage.MyMessage1";
}

MyMessage1& MyMessage1::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage1::s() const noexcept {
    return m_s;
}

}
)";

    const char *EXPECTED_HEADER2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE_MYMESSAGE2_HPP
#define MYPACKAGE_MYMESSAGE2_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage2 {
    public:
        MyMessage2() = default;
        MyMessage2(const MyMessage2&) = default;
        MyMessage2& operator=(const MyMessage2&) = default;
        MyMessage2(MyMessage2&&) = default; // NOLINT
        MyMessage2& operator=(MyMessage2&&) = default; // NOLINT
        ~MyMessage2() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage2& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}

template<>
struct isVisitable<MyPackage::MyMessage2> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage::MyMessage2> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage {

int32_t MyMessage2::ID() {
    return 2;
}

const std::string MyMessage2::ShortName() {
    return "MyMessage2";
}
const std::string MyMessage2::LongName() {
    return "MyPackage.MyMessage2";
}

MyMessage2& MyMessage2::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage2::s() const noexcept {
    return m_s;
}

}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());
    {
        auto firstMetaMessage = retVal.first.front();
        cluon::MetaMessageToCPPTransformator t;
        firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        //        std::cout << t.contentHeader() << std::endl;
        //        std::cout << t.contentSource() << std::endl;
        REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER1));
        REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE1));
    }
    {
        auto secondMetaMessage = retVal.first.back();
        cluon::MetaMessageToCPPTransformator t;
        secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        //        std::cout << t.contentHeader() << std::endl;
        //        std::cout << t.contentSource() << std::endl;
        REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER2));
        REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE2));
    }
}

TEST_CASE("Transforming two messages with package and sub.package and one field.") {
    const char *input = R"(
package MyPackage.SubPackage;
message MyMessage1 [id = 1] {
    string s [id = 1];
}

message MyMessage2 [id = 2] {
    string s [id = 1];
}
)";

    const char *EXPECTED_HEADER1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE_SUBPACKAGE_MYMESSAGE1_HPP
#define MYPACKAGE_SUBPACKAGE_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage { namespace SubPackage {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}}

template<>
struct isVisitable<MyPackage::SubPackage::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage::SubPackage::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage { namespace SubPackage {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyPackage.SubPackage.MyMessage1";
}

MyMessage1& MyMessage1::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage1::s() const noexcept {
    return m_s;
}

}}
)";

    const char *EXPECTED_HEADER2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE_SUBPACKAGE_MYMESSAGE2_HPP
#define MYPACKAGE_SUBPACKAGE_MYMESSAGE2_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage { namespace SubPackage {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage2 {
    public:
        MyMessage2() = default;
        MyMessage2(const MyMessage2&) = default;
        MyMessage2& operator=(const MyMessage2&) = default;
        MyMessage2(MyMessage2&&) = default; // NOLINT
        MyMessage2& operator=(MyMessage2&&) = default; // NOLINT
        ~MyMessage2() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage2& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}}

template<>
struct isVisitable<MyPackage::SubPackage::MyMessage2> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage::SubPackage::MyMessage2> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage { namespace SubPackage {

int32_t MyMessage2::ID() {
    return 2;
}

const std::string MyMessage2::ShortName() {
    return "MyMessage2";
}
const std::string MyMessage2::LongName() {
    return "MyPackage.SubPackage.MyMessage2";
}

MyMessage2& MyMessage2::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage2::s() const noexcept {
    return m_s;
}

}}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);
    REQUIRE(2 == retVal.first.size());
    {
        auto firstMetaMessage = retVal.first.front();
        cluon::MetaMessageToCPPTransformator t;
        firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        //        std::cout << t.contentHeader() << std::endl;
        //        std::cout << t.contentSource() << std::endl;
        REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER1));
        REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE1));
    }
    {
        auto secondMetaMessage = retVal.first.back();
        cluon::MetaMessageToCPPTransformator t;
        secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
        //        std::cout << t.contentHeader() << std::endl;
        //        std::cout << t.contentSource() << std::endl;
        REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER2));
        REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE2));
    }
}

TEST_CASE("Transforming one message with package and one field.") {
    const char *input = R"(
package MyPackage1;
message MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_HEADER = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE1_MYMESSAGE1_HPP
#define MYPACKAGE1_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage1 {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}

template<>
struct isVisitable<MyPackage1::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage1::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage1 {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyPackage1.MyMessage1";
}

MyMessage1& MyMessage1::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage1::s() const noexcept {
    return m_s;
}

}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE));
}

TEST_CASE("Transforming one message with multiple packages and one field.") {
    const char *input = R"(
package MyPackage1.MySubPackage;
message MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_HEADER = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE1_MYSUBPACKAGE_MYMESSAGE1_HPP
#define MYPACKAGE1_MYSUBPACKAGE_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage1 { namespace MySubPackage {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}}

template<>
struct isVisitable<MyPackage1::MySubPackage::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage1::MySubPackage::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage1 { namespace MySubPackage {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyPackage1.MySubPackage.MyMessage1";
}

MyMessage1& MyMessage1::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage1::s() const noexcept {
    return m_s;
}

}}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE));
}

TEST_CASE("Transforming one message with sub-package with package and one field.") {
    const char *input = R"(
package MyPackage1;
message MySubPackageMessage.MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_HEADER = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE1_MYSUBPACKAGEMESSAGE_MYMESSAGE1_HPP
#define MYPACKAGE1_MYSUBPACKAGEMESSAGE_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage1 { namespace MySubPackageMessage {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}}

template<>
struct isVisitable<MyPackage1::MySubPackageMessage::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage1::MySubPackageMessage::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage1 { namespace MySubPackageMessage {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyPackage1.MySubPackageMessage.MyMessage1";
}

MyMessage1& MyMessage1::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage1::s() const noexcept {
    return m_s;
}

}}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE));
}

TEST_CASE("Transforming one message with sub-package with multiple packages "
          "and one field.") {
    const char *input = R"(
package MyPackage1.MySubPackage;
message MySubPackageMessage.MyMessage1 [id = 1] {
    string s [id = 1];
}
)";

    const char *EXPECTED_HEADER = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYPACKAGE1_MYSUBPACKAGE_MYSUBPACKAGEMESSAGE_MYMESSAGE1_HPP
#define MYPACKAGE1_MYSUBPACKAGE_MYSUBPACKAGEMESSAGE_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace MyPackage1 { namespace MySubPackage { namespace MySubPackageMessage {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& s(const std::string &v) noexcept;
        std::string s() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("s"s), m_s, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("s"s), m_s, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_s{ ""s }; // field identifier = 1.
        
};
}}}

template<>
struct isVisitable<MyPackage1::MySubPackage::MySubPackageMessage::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyPackage1::MySubPackage::MySubPackageMessage::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace MyPackage1 { namespace MySubPackage { namespace MySubPackageMessage {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyPackage1.MySubPackage.MySubPackageMessage.MyMessage1";
}

MyMessage1& MyMessage1::s(const std::string &v) noexcept {
    m_s = v;
    return *this;
}
std::string MyMessage1::s() const noexcept {
    return m_s;
}

}}}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE));
}

TEST_CASE("Transforming one message with and all fields.") {
    const char *input = R"(
message MyMessage1 [id = 1] {
    bool attribute1 [id = 1];
    char attribute2 [id = 2];
    int32 attribute3 [id = 3];
    uint32 attribute4 [id = 4];
    float attribute5 [id = 5];
    double attribute6 [id = 6];
    string attribute7 [id = 7];
}
)";

    const char *EXPECTED_HEADER = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYMESSAGE1_HPP
#define MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>

using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& attribute1(const bool &v) noexcept;
        bool attribute1() const noexcept;
        
        MyMessage1& attribute2(const char &v) noexcept;
        char attribute2() const noexcept;
        
        MyMessage1& attribute3(const int32_t &v) noexcept;
        int32_t attribute3() const noexcept;
        
        MyMessage1& attribute4(const uint32_t &v) noexcept;
        uint32_t attribute4() const noexcept;
        
        MyMessage1& attribute5(const float &v) noexcept;
        float attribute5() const noexcept;
        
        MyMessage1& attribute6(const double &v) noexcept;
        double attribute6() const noexcept;
        
        MyMessage1& attribute7(const std::string &v) noexcept;
        std::string attribute7() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("bool"s), std::move("attribute1"s), m_attribute1, visitor);
            
            doVisit(2, std::move("char"s), std::move("attribute2"s), m_attribute2, visitor);
            
            doVisit(3, std::move("int32_t"s), std::move("attribute3"s), m_attribute3, visitor);
            
            doVisit(4, std::move("uint32_t"s), std::move("attribute4"s), m_attribute4, visitor);
            
            doVisit(5, std::move("float"s), std::move("attribute5"s), m_attribute5, visitor);
            
            doVisit(6, std::move("double"s), std::move("attribute6"s), m_attribute6, visitor);
            
            doVisit(7, std::move("std::string"s), std::move("attribute7"s), m_attribute7, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("bool"s), std::move("attribute1"s), m_attribute1, preVisit, visit, postVisit);
            
            doTripletForwardVisit(2, std::move("char"s), std::move("attribute2"s), m_attribute2, preVisit, visit, postVisit);
            
            doTripletForwardVisit(3, std::move("int32_t"s), std::move("attribute3"s), m_attribute3, preVisit, visit, postVisit);
            
            doTripletForwardVisit(4, std::move("uint32_t"s), std::move("attribute4"s), m_attribute4, preVisit, visit, postVisit);
            
            doTripletForwardVisit(5, std::move("float"s), std::move("attribute5"s), m_attribute5, preVisit, visit, postVisit);
            
            doTripletForwardVisit(6, std::move("double"s), std::move("attribute6"s), m_attribute6, preVisit, visit, postVisit);
            
            doTripletForwardVisit(7, std::move("std::string"s), std::move("attribute7"s), m_attribute7, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        bool m_attribute1{ false }; // field identifier = 1.
        
        char m_attribute2{ '\0' }; // field identifier = 2.
        
        int32_t m_attribute3{ 0 }; // field identifier = 3.
        
        uint32_t m_attribute4{ 0 }; // field identifier = 4.
        
        float m_attribute5{ 0.0f }; // field identifier = 5.
        
        double m_attribute6{ 0.0 }; // field identifier = 6.
        
        std::string m_attribute7{ ""s }; // field identifier = 7.
        
};


template<>
struct isVisitable<MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */


int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyMessage1";
}

MyMessage1& MyMessage1::attribute1(const bool &v) noexcept {
    m_attribute1 = v;
    return *this;
}
bool MyMessage1::attribute1() const noexcept {
    return m_attribute1;
}

MyMessage1& MyMessage1::attribute2(const char &v) noexcept {
    m_attribute2 = v;
    return *this;
}
char MyMessage1::attribute2() const noexcept {
    return m_attribute2;
}

MyMessage1& MyMessage1::attribute3(const int32_t &v) noexcept {
    m_attribute3 = v;
    return *this;
}
int32_t MyMessage1::attribute3() const noexcept {
    return m_attribute3;
}

MyMessage1& MyMessage1::attribute4(const uint32_t &v) noexcept {
    m_attribute4 = v;
    return *this;
}
uint32_t MyMessage1::attribute4() const noexcept {
    return m_attribute4;
}

MyMessage1& MyMessage1::attribute5(const float &v) noexcept {
    m_attribute5 = v;
    return *this;
}
float MyMessage1::attribute5() const noexcept {
    return m_attribute5;
}

MyMessage1& MyMessage1::attribute6(const double &v) noexcept {
    m_attribute6 = v;
    return *this;
}
double MyMessage1::attribute6() const noexcept {
    return m_attribute6;
}

MyMessage1& MyMessage1::attribute7(const std::string &v) noexcept {
    m_attribute7 = v;
    return *this;
}
std::string MyMessage1::attribute7() const noexcept {
    return m_attribute7;
}


)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE));
}

TEST_CASE("Transforming one message with all fields and individual initializers.") {
    const char *input = R"(
message MyMessage1 [id = 1] {
    bool attribute1 [default = true, id = 1];
    char attribute2 [default = 'c', id = 2];
    int8 attribute3 [default = -1, id = 3];
    uint8 attribute4 [default = 2, id = 4];
    int16 attribute5 [default = -3, id = 5];
    uint16 attribute6 [default = 4, id = 6];
    int32 attribute7 [default = -5, id = 7];
    uint32 attribute8 [default = 6, id = 8];
    int64 attribute9 [default = -7, id = 9];
    uint64 attribute10 [default = 8, id = 10];
    float attribute11 [default = -9.5, id = 11];
    double attribute12 [default = 10.6, id = 12];
    string attribute13 [default = "Hello World", id = 13];
    bytes attribute14 [default = "Hello Galaxy", id = 14];
}
)";

    const char *EXPECTED_HEADER = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYMESSAGE1_HPP
#define MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>

using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& attribute1(const bool &v) noexcept;
        bool attribute1() const noexcept;
        
        MyMessage1& attribute2(const char &v) noexcept;
        char attribute2() const noexcept;
        
        MyMessage1& attribute3(const int8_t &v) noexcept;
        int8_t attribute3() const noexcept;
        
        MyMessage1& attribute4(const uint8_t &v) noexcept;
        uint8_t attribute4() const noexcept;
        
        MyMessage1& attribute5(const int16_t &v) noexcept;
        int16_t attribute5() const noexcept;
        
        MyMessage1& attribute6(const uint16_t &v) noexcept;
        uint16_t attribute6() const noexcept;
        
        MyMessage1& attribute7(const int32_t &v) noexcept;
        int32_t attribute7() const noexcept;
        
        MyMessage1& attribute8(const uint32_t &v) noexcept;
        uint32_t attribute8() const noexcept;
        
        MyMessage1& attribute9(const int64_t &v) noexcept;
        int64_t attribute9() const noexcept;
        
        MyMessage1& attribute10(const uint64_t &v) noexcept;
        uint64_t attribute10() const noexcept;
        
        MyMessage1& attribute11(const float &v) noexcept;
        float attribute11() const noexcept;
        
        MyMessage1& attribute12(const double &v) noexcept;
        double attribute12() const noexcept;
        
        MyMessage1& attribute13(const std::string &v) noexcept;
        std::string attribute13() const noexcept;
        
        MyMessage1& attribute14(const std::string &v) noexcept;
        std::string attribute14() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("bool"s), std::move("attribute1"s), m_attribute1, visitor);
            
            doVisit(2, std::move("char"s), std::move("attribute2"s), m_attribute2, visitor);
            
            doVisit(3, std::move("int8_t"s), std::move("attribute3"s), m_attribute3, visitor);
            
            doVisit(4, std::move("uint8_t"s), std::move("attribute4"s), m_attribute4, visitor);
            
            doVisit(5, std::move("int16_t"s), std::move("attribute5"s), m_attribute5, visitor);
            
            doVisit(6, std::move("uint16_t"s), std::move("attribute6"s), m_attribute6, visitor);
            
            doVisit(7, std::move("int32_t"s), std::move("attribute7"s), m_attribute7, visitor);
            
            doVisit(8, std::move("uint32_t"s), std::move("attribute8"s), m_attribute8, visitor);
            
            doVisit(9, std::move("int64_t"s), std::move("attribute9"s), m_attribute9, visitor);
            
            doVisit(10, std::move("uint64_t"s), std::move("attribute10"s), m_attribute10, visitor);
            
            doVisit(11, std::move("float"s), std::move("attribute11"s), m_attribute11, visitor);
            
            doVisit(12, std::move("double"s), std::move("attribute12"s), m_attribute12, visitor);
            
            doVisit(13, std::move("std::string"s), std::move("attribute13"s), m_attribute13, visitor);
            
            doVisit(14, std::move("std::string"s), std::move("attribute14"s), m_attribute14, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("bool"s), std::move("attribute1"s), m_attribute1, preVisit, visit, postVisit);
            
            doTripletForwardVisit(2, std::move("char"s), std::move("attribute2"s), m_attribute2, preVisit, visit, postVisit);
            
            doTripletForwardVisit(3, std::move("int8_t"s), std::move("attribute3"s), m_attribute3, preVisit, visit, postVisit);
            
            doTripletForwardVisit(4, std::move("uint8_t"s), std::move("attribute4"s), m_attribute4, preVisit, visit, postVisit);
            
            doTripletForwardVisit(5, std::move("int16_t"s), std::move("attribute5"s), m_attribute5, preVisit, visit, postVisit);
            
            doTripletForwardVisit(6, std::move("uint16_t"s), std::move("attribute6"s), m_attribute6, preVisit, visit, postVisit);
            
            doTripletForwardVisit(7, std::move("int32_t"s), std::move("attribute7"s), m_attribute7, preVisit, visit, postVisit);
            
            doTripletForwardVisit(8, std::move("uint32_t"s), std::move("attribute8"s), m_attribute8, preVisit, visit, postVisit);
            
            doTripletForwardVisit(9, std::move("int64_t"s), std::move("attribute9"s), m_attribute9, preVisit, visit, postVisit);
            
            doTripletForwardVisit(10, std::move("uint64_t"s), std::move("attribute10"s), m_attribute10, preVisit, visit, postVisit);
            
            doTripletForwardVisit(11, std::move("float"s), std::move("attribute11"s), m_attribute11, preVisit, visit, postVisit);
            
            doTripletForwardVisit(12, std::move("double"s), std::move("attribute12"s), m_attribute12, preVisit, visit, postVisit);
            
            doTripletForwardVisit(13, std::move("std::string"s), std::move("attribute13"s), m_attribute13, preVisit, visit, postVisit);
            
            doTripletForwardVisit(14, std::move("std::string"s), std::move("attribute14"s), m_attribute14, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        bool m_attribute1{ true }; // field identifier = 1.
        
        char m_attribute2{ 'c' }; // field identifier = 2.
        
        int8_t m_attribute3{ -1 }; // field identifier = 3.
        
        uint8_t m_attribute4{ 2 }; // field identifier = 4.
        
        int16_t m_attribute5{ -3 }; // field identifier = 5.
        
        uint16_t m_attribute6{ 4 }; // field identifier = 6.
        
        int32_t m_attribute7{ -5 }; // field identifier = 7.
        
        uint32_t m_attribute8{ 6 }; // field identifier = 8.
        
        int64_t m_attribute9{ -7 }; // field identifier = 9.
        
        uint64_t m_attribute10{ 8 }; // field identifier = 10.
        
        float m_attribute11{ -9.5f }; // field identifier = 11.
        
        double m_attribute12{ 10.6 }; // field identifier = 12.
        
        std::string m_attribute13{ "Hello World"s }; // field identifier = 13.
        
        std::string m_attribute14{ "Hello Galaxy"s }; // field identifier = 14.
        
};


template<>
struct isVisitable<MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */


int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyMessage1";
}

MyMessage1& MyMessage1::attribute1(const bool &v) noexcept {
    m_attribute1 = v;
    return *this;
}
bool MyMessage1::attribute1() const noexcept {
    return m_attribute1;
}

MyMessage1& MyMessage1::attribute2(const char &v) noexcept {
    m_attribute2 = v;
    return *this;
}
char MyMessage1::attribute2() const noexcept {
    return m_attribute2;
}

MyMessage1& MyMessage1::attribute3(const int8_t &v) noexcept {
    m_attribute3 = v;
    return *this;
}
int8_t MyMessage1::attribute3() const noexcept {
    return m_attribute3;
}

MyMessage1& MyMessage1::attribute4(const uint8_t &v) noexcept {
    m_attribute4 = v;
    return *this;
}
uint8_t MyMessage1::attribute4() const noexcept {
    return m_attribute4;
}

MyMessage1& MyMessage1::attribute5(const int16_t &v) noexcept {
    m_attribute5 = v;
    return *this;
}
int16_t MyMessage1::attribute5() const noexcept {
    return m_attribute5;
}

MyMessage1& MyMessage1::attribute6(const uint16_t &v) noexcept {
    m_attribute6 = v;
    return *this;
}
uint16_t MyMessage1::attribute6() const noexcept {
    return m_attribute6;
}

MyMessage1& MyMessage1::attribute7(const int32_t &v) noexcept {
    m_attribute7 = v;
    return *this;
}
int32_t MyMessage1::attribute7() const noexcept {
    return m_attribute7;
}

MyMessage1& MyMessage1::attribute8(const uint32_t &v) noexcept {
    m_attribute8 = v;
    return *this;
}
uint32_t MyMessage1::attribute8() const noexcept {
    return m_attribute8;
}

MyMessage1& MyMessage1::attribute9(const int64_t &v) noexcept {
    m_attribute9 = v;
    return *this;
}
int64_t MyMessage1::attribute9() const noexcept {
    return m_attribute9;
}

MyMessage1& MyMessage1::attribute10(const uint64_t &v) noexcept {
    m_attribute10 = v;
    return *this;
}
uint64_t MyMessage1::attribute10() const noexcept {
    return m_attribute10;
}

MyMessage1& MyMessage1::attribute11(const float &v) noexcept {
    m_attribute11 = v;
    return *this;
}
float MyMessage1::attribute11() const noexcept {
    return m_attribute11;
}

MyMessage1& MyMessage1::attribute12(const double &v) noexcept {
    m_attribute12 = v;
    return *this;
}
double MyMessage1::attribute12() const noexcept {
    return m_attribute12;
}

MyMessage1& MyMessage1::attribute13(const std::string &v) noexcept {
    m_attribute13 = v;
    return *this;
}
std::string MyMessage1::attribute13() const noexcept {
    return m_attribute13;
}

MyMessage1& MyMessage1::attribute14(const std::string &v) noexcept {
    m_attribute14 = v;
    return *this;
}
std::string MyMessage1::attribute14() const noexcept {
    return m_attribute14;
}


)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto firstMetaMessage = retVal.first.front();
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE));
}

TEST_CASE("Transforming two messages with one field each and cross-referenced message type.") {
    const char *input = R"(
message MyMessage1 [id = 1] {
    string attribute1 [id = 1];
}

message MyMessage2 [id = 2] {
    uint8 field1 [id = 1];
    MyMessage1 field2 [id = 2];
}
)";

    const char *EXPECTED_HEADER1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYMESSAGE1_HPP
#define MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>

using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& attribute1(const std::string &v) noexcept;
        std::string attribute1() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_attribute1{ ""s }; // field identifier = 1.
        
};


template<>
struct isVisitable<MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */


int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "MyMessage1";
}

MyMessage1& MyMessage1::attribute1(const std::string &v) noexcept {
    m_attribute1 = v;
    return *this;
}
std::string MyMessage1::attribute1() const noexcept {
    return m_attribute1;
}


)";

    const char *EXPECTED_HEADER2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef MYMESSAGE2_HPP
#define MYMESSAGE2_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>

using namespace std::string_literals; // NOLINT
class LIB_API MyMessage2 {
    public:
        MyMessage2() = default;
        MyMessage2(const MyMessage2&) = default;
        MyMessage2& operator=(const MyMessage2&) = default;
        MyMessage2(MyMessage2&&) = default; // NOLINT
        MyMessage2& operator=(MyMessage2&&) = default; // NOLINT
        ~MyMessage2() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage2& field1(const uint8_t &v) noexcept;
        uint8_t field1() const noexcept;
        
        MyMessage2& field2(const MyMessage1 &v) noexcept;
        MyMessage1 field2() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, visitor);
            
            doVisit(2, std::move("MyMessage1"s), std::move("field2"s), m_field2, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, preVisit, visit, postVisit);
            
            doTripletForwardVisit(2, std::move("MyMessage1"s), std::move("field2"s), m_field2, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        uint8_t m_field1{ 0 }; // field identifier = 1.
        
        MyMessage1 m_field2{  }; // field identifier = 2.
        
};


template<>
struct isVisitable<MyMessage2> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<MyMessage2> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */


int32_t MyMessage2::ID() {
    return 2;
}

const std::string MyMessage2::ShortName() {
    return "MyMessage2";
}
const std::string MyMessage2::LongName() {
    return "MyMessage2";
}

MyMessage2& MyMessage2::field1(const uint8_t &v) noexcept {
    m_field1 = v;
    return *this;
}
uint8_t MyMessage2::field1() const noexcept {
    return m_field1;
}

MyMessage2& MyMessage2::field2(const MyMessage1 &v) noexcept {
    m_field2 = v;
    return *this;
}
MyMessage1 MyMessage2::field2() const noexcept {
    return m_field2;
}


)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto listOfMessages = retVal.first;
    REQUIRE(2 == listOfMessages.size());

    auto firstMetaMessage  = listOfMessages[0];
    auto secondMetaMessage = listOfMessages[1];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER1));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE1));

    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER2));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE2));
}

TEST_CASE("Transforming two messages with one field each and cross-referenced message type with sub-message name.") {
    const char *input = R"(
message DEF.MyMessage1 [id = 1] {
    string attribute1 [id = 1];
}

message DEF.MyMessage2 [id = 2] {
    uint8 field1 [id = 1];
    DEF.MyMessage1 field2 [id = 2];
}
)";

    const char *EXPECTED_HEADER1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef DEF_MYMESSAGE1_HPP
#define DEF_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace DEF {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& attribute1(const std::string &v) noexcept;
        std::string attribute1() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_attribute1{ ""s }; // field identifier = 1.
        
};
}

template<>
struct isVisitable<DEF::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<DEF::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace DEF {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "DEF.MyMessage1";
}

MyMessage1& MyMessage1::attribute1(const std::string &v) noexcept {
    m_attribute1 = v;
    return *this;
}
std::string MyMessage1::attribute1() const noexcept {
    return m_attribute1;
}

}
)";

    const char *EXPECTED_HEADER2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef DEF_MYMESSAGE2_HPP
#define DEF_MYMESSAGE2_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace DEF {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage2 {
    public:
        MyMessage2() = default;
        MyMessage2(const MyMessage2&) = default;
        MyMessage2& operator=(const MyMessage2&) = default;
        MyMessage2(MyMessage2&&) = default; // NOLINT
        MyMessage2& operator=(MyMessage2&&) = default; // NOLINT
        ~MyMessage2() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage2& field1(const uint8_t &v) noexcept;
        uint8_t field1() const noexcept;
        
        MyMessage2& field2(const DEF::MyMessage1 &v) noexcept;
        DEF::MyMessage1 field2() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, visitor);
            
            doVisit(2, std::move("DEF::MyMessage1"s), std::move("field2"s), m_field2, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, preVisit, visit, postVisit);
            
            doTripletForwardVisit(2, std::move("DEF::MyMessage1"s), std::move("field2"s), m_field2, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        uint8_t m_field1{ 0 }; // field identifier = 1.
        
        DEF::MyMessage1 m_field2{  }; // field identifier = 2.
        
};
}

template<>
struct isVisitable<DEF::MyMessage2> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<DEF::MyMessage2> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace DEF {

int32_t MyMessage2::ID() {
    return 2;
}

const std::string MyMessage2::ShortName() {
    return "MyMessage2";
}
const std::string MyMessage2::LongName() {
    return "DEF.MyMessage2";
}

MyMessage2& MyMessage2::field1(const uint8_t &v) noexcept {
    m_field1 = v;
    return *this;
}
uint8_t MyMessage2::field1() const noexcept {
    return m_field1;
}

MyMessage2& MyMessage2::field2(const DEF::MyMessage1 &v) noexcept {
    m_field2 = v;
    return *this;
}
DEF::MyMessage1 MyMessage2::field2() const noexcept {
    return m_field2;
}

}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto listOfMessages = retVal.first;
    REQUIRE(2 == listOfMessages.size());

    auto firstMetaMessage  = listOfMessages[0];
    auto secondMetaMessage = listOfMessages[1];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER1));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE1));

    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER2));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE2));
}

TEST_CASE("Transforming two messages with one field each and cross-referenced message type with package name.") {
    const char *input = R"(
package DEF;
message MyMessage1 [id = 1] {
    string attribute1 [id = 1];
}

message MyMessage2 [id = 2] {
    uint8 field1 [id = 1];
    MyMessage1 field2 [id = 2];
}
)";

    const char *EXPECTED_HEADER1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef DEF_MYMESSAGE1_HPP
#define DEF_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace DEF {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& attribute1(const std::string &v) noexcept;
        std::string attribute1() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_attribute1{ ""s }; // field identifier = 1.
        
};
}

template<>
struct isVisitable<DEF::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<DEF::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace DEF {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "DEF.MyMessage1";
}

MyMessage1& MyMessage1::attribute1(const std::string &v) noexcept {
    m_attribute1 = v;
    return *this;
}
std::string MyMessage1::attribute1() const noexcept {
    return m_attribute1;
}

}
)";

    const char *EXPECTED_HEADER2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef DEF_MYMESSAGE2_HPP
#define DEF_MYMESSAGE2_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace DEF {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage2 {
    public:
        MyMessage2() = default;
        MyMessage2(const MyMessage2&) = default;
        MyMessage2& operator=(const MyMessage2&) = default;
        MyMessage2(MyMessage2&&) = default; // NOLINT
        MyMessage2& operator=(MyMessage2&&) = default; // NOLINT
        ~MyMessage2() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage2& field1(const uint8_t &v) noexcept;
        uint8_t field1() const noexcept;
        
        MyMessage2& field2(const DEF::MyMessage1 &v) noexcept;
        DEF::MyMessage1 field2() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, visitor);
            
            doVisit(2, std::move("DEF::MyMessage1"s), std::move("field2"s), m_field2, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, preVisit, visit, postVisit);
            
            doTripletForwardVisit(2, std::move("DEF::MyMessage1"s), std::move("field2"s), m_field2, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        uint8_t m_field1{ 0 }; // field identifier = 1.
        
        DEF::MyMessage1 m_field2{  }; // field identifier = 2.
        
};
}

template<>
struct isVisitable<DEF::MyMessage2> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<DEF::MyMessage2> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace DEF {

int32_t MyMessage2::ID() {
    return 2;
}

const std::string MyMessage2::ShortName() {
    return "MyMessage2";
}
const std::string MyMessage2::LongName() {
    return "DEF.MyMessage2";
}

MyMessage2& MyMessage2::field1(const uint8_t &v) noexcept {
    m_field1 = v;
    return *this;
}
uint8_t MyMessage2::field1() const noexcept {
    return m_field1;
}

MyMessage2& MyMessage2::field2(const DEF::MyMessage1 &v) noexcept {
    m_field2 = v;
    return *this;
}
DEF::MyMessage1 MyMessage2::field2() const noexcept {
    return m_field2;
}

}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto listOfMessages = retVal.first;
    REQUIRE(2 == listOfMessages.size());

    auto firstMetaMessage  = listOfMessages[0];
    auto secondMetaMessage = listOfMessages[1];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER1));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE1));

    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER2));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE2));
}

TEST_CASE("Transforming two messages with one field each and cross-referenced message type and sub-names with package "
          "name.") {
    const char *input = R"(
package ABC;
message DEF.MyMessage1 [id = 1] {
    string attribute1 [id = 1];
}

message DEF.MyMessage2 [id = 2] {
    uint8 field1 [id = 1];
    DEF.MyMessage1 field2 [id = 2];
}
)";

    const char *EXPECTED_HEADER1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef ABC_DEF_MYMESSAGE1_HPP
#define ABC_DEF_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace ABC { namespace DEF {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& attribute1(const std::string &v) noexcept;
        std::string attribute1() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_attribute1{ ""s }; // field identifier = 1.
        
};
}}

template<>
struct isVisitable<ABC::DEF::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<ABC::DEF::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace ABC { namespace DEF {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "ABC.DEF.MyMessage1";
}

MyMessage1& MyMessage1::attribute1(const std::string &v) noexcept {
    m_attribute1 = v;
    return *this;
}
std::string MyMessage1::attribute1() const noexcept {
    return m_attribute1;
}

}}
)";

    const char *EXPECTED_HEADER2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef ABC_DEF_MYMESSAGE2_HPP
#define ABC_DEF_MYMESSAGE2_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace ABC { namespace DEF {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage2 {
    public:
        MyMessage2() = default;
        MyMessage2(const MyMessage2&) = default;
        MyMessage2& operator=(const MyMessage2&) = default;
        MyMessage2(MyMessage2&&) = default; // NOLINT
        MyMessage2& operator=(MyMessage2&&) = default; // NOLINT
        ~MyMessage2() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage2& field1(const uint8_t &v) noexcept;
        uint8_t field1() const noexcept;
        
        MyMessage2& field2(const ABC::DEF::MyMessage1 &v) noexcept;
        ABC::DEF::MyMessage1 field2() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, visitor);
            
            doVisit(2, std::move("ABC::DEF::MyMessage1"s), std::move("field2"s), m_field2, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, preVisit, visit, postVisit);
            
            doTripletForwardVisit(2, std::move("ABC::DEF::MyMessage1"s), std::move("field2"s), m_field2, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        uint8_t m_field1{ 0 }; // field identifier = 1.
        
        ABC::DEF::MyMessage1 m_field2{  }; // field identifier = 2.
        
};
}}

template<>
struct isVisitable<ABC::DEF::MyMessage2> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<ABC::DEF::MyMessage2> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace ABC { namespace DEF {

int32_t MyMessage2::ID() {
    return 2;
}

const std::string MyMessage2::ShortName() {
    return "MyMessage2";
}
const std::string MyMessage2::LongName() {
    return "ABC.DEF.MyMessage2";
}

MyMessage2& MyMessage2::field1(const uint8_t &v) noexcept {
    m_field1 = v;
    return *this;
}
uint8_t MyMessage2::field1() const noexcept {
    return m_field1;
}

MyMessage2& MyMessage2::field2(const ABC::DEF::MyMessage1 &v) noexcept {
    m_field2 = v;
    return *this;
}
ABC::DEF::MyMessage1 MyMessage2::field2() const noexcept {
    return m_field2;
}

}}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto listOfMessages = retVal.first;
    REQUIRE(2 == listOfMessages.size());

    auto firstMetaMessage  = listOfMessages[0];
    auto secondMetaMessage = listOfMessages[1];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER1));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE1));

    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER2));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE2));
}

TEST_CASE("Transforming two messages with one field each and cross-referenced message type and sub-names with package "
          "name with sub-package.") {
    const char *input = R"(
package ABC.GHI;
message DEF.MyMessage1 [id = 1] {
    string attribute1 [id = 1];
}

message DEF.MyMessage2 [id = 2] {
    uint8 field1 [id = 1];
    DEF.MyMessage1 field2 [id = 2];
}
)";

    const char *EXPECTED_HEADER1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef ABC_GHI_DEF_MYMESSAGE1_HPP
#define ABC_GHI_DEF_MYMESSAGE1_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace ABC { namespace GHI { namespace DEF {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage1 {
    public:
        MyMessage1() = default;
        MyMessage1(const MyMessage1&) = default;
        MyMessage1& operator=(const MyMessage1&) = default;
        MyMessage1(MyMessage1&&) = default; // NOLINT
        MyMessage1& operator=(MyMessage1&&) = default; // NOLINT
        ~MyMessage1() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage1& attribute1(const std::string &v) noexcept;
        std::string attribute1() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("std::string"s), std::move("attribute1"s), m_attribute1, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        std::string m_attribute1{ ""s }; // field identifier = 1.
        
};
}}}

template<>
struct isVisitable<ABC::GHI::DEF::MyMessage1> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<ABC::GHI::DEF::MyMessage1> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE1 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace ABC { namespace GHI { namespace DEF {

int32_t MyMessage1::ID() {
    return 1;
}

const std::string MyMessage1::ShortName() {
    return "MyMessage1";
}
const std::string MyMessage1::LongName() {
    return "ABC.GHI.DEF.MyMessage1";
}

MyMessage1& MyMessage1::attribute1(const std::string &v) noexcept {
    m_attribute1 = v;
    return *this;
}
std::string MyMessage1::attribute1() const noexcept {
    return m_attribute1;
}

}}}
)";

    const char *EXPECTED_HEADER2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef ABC_GHI_DEF_MYMESSAGE2_HPP
#define ABC_GHI_DEF_MYMESSAGE2_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>
namespace ABC { namespace GHI { namespace DEF {
using namespace std::string_literals; // NOLINT
class LIB_API MyMessage2 {
    public:
        MyMessage2() = default;
        MyMessage2(const MyMessage2&) = default;
        MyMessage2& operator=(const MyMessage2&) = default;
        MyMessage2(MyMessage2&&) = default; // NOLINT
        MyMessage2& operator=(MyMessage2&&) = default; // NOLINT
        ~MyMessage2() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyMessage2& field1(const uint8_t &v) noexcept;
        uint8_t field1() const noexcept;
        
        MyMessage2& field2(const ABC::GHI::DEF::MyMessage1 &v) noexcept;
        ABC::GHI::DEF::MyMessage1 field2() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, visitor);
            
            doVisit(2, std::move("ABC::GHI::DEF::MyMessage1"s), std::move("field2"s), m_field2, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("uint8_t"s), std::move("field1"s), m_field1, preVisit, visit, postVisit);
            
            doTripletForwardVisit(2, std::move("ABC::GHI::DEF::MyMessage1"s), std::move("field2"s), m_field2, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        uint8_t m_field1{ 0 }; // field identifier = 1.
        
        ABC::GHI::DEF::MyMessage1 m_field2{  }; // field identifier = 2.
        
};
}}}

template<>
struct isVisitable<ABC::GHI::DEF::MyMessage2> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<ABC::GHI::DEF::MyMessage2> {
    static const bool value = true;
};
#endif
)";

    const char *EXPECTED_SOURCE2 = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace ABC { namespace GHI { namespace DEF {

int32_t MyMessage2::ID() {
    return 2;
}

const std::string MyMessage2::ShortName() {
    return "MyMessage2";
}
const std::string MyMessage2::LongName() {
    return "ABC.GHI.DEF.MyMessage2";
}

MyMessage2& MyMessage2::field1(const uint8_t &v) noexcept {
    m_field1 = v;
    return *this;
}
uint8_t MyMessage2::field1() const noexcept {
    return m_field1;
}

MyMessage2& MyMessage2::field2(const ABC::GHI::DEF::MyMessage1 &v) noexcept {
    m_field2 = v;
    return *this;
}
ABC::GHI::DEF::MyMessage1 MyMessage2::field2() const noexcept {
    return m_field2;
}

}}}
)";

    cluon::MessageParser mp;
    auto retVal = mp.parse(std::string(input));
    REQUIRE(cluon::MessageParser::MessageParserErrorCodes::NO_ERROR == retVal.second);

    cluon::MetaMessageToCPPTransformator t;
    auto listOfMessages = retVal.first;
    REQUIRE(2 == listOfMessages.size());

    auto firstMetaMessage  = listOfMessages[0];
    auto secondMetaMessage = listOfMessages[1];
    firstMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER1));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE1));

    secondMetaMessage.accept([&trans = t](const cluon::MetaMessage &_mm) { trans.visit(_mm); });
    //    std::cout << t.contentHeader() << std::endl;
    //    std::cout << t.contentSource() << std::endl;
    REQUIRE(t.contentHeader() == std::string(EXPECTED_HEADER2));
    REQUIRE(t.contentSource() == std::string(EXPECTED_SOURCE2));
}
