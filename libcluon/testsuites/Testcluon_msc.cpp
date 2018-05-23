/*
 * Copyright (C) 2018  Christian Berger
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

#include "cluon-msc.hpp"

#include <fstream>
#include <sstream>
#include <string>

// clang-format off
#ifdef WIN32
    #define UNLINK _unlink
#else
    #include <unistd.h>
    #define UNLINK unlink
#endif
// clang-format on

TEST_CASE("Test empty commandline parameters.") {
    int32_t argc       = 1;
    const char *argv[] = {static_cast<const char *>("cluon-msc")};
    REQUIRE(1 == cluon_msc(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test no conversion with non-existing ODVD.") {
    constexpr int32_t argc = 2;
    const char *argv[]     = {static_cast<const char *>("cluon-msc"), static_cast<const char *>("ABCnonexisting.odvd")};
    REQUIRE(1 == cluon_msc(argc, const_cast<char **>(argv)));
}

TEST_CASE("Test no conversion with corrupt ODVD.") {
    UNLINK("ABC0.odvd");
    UNLINK("ABC0.out");

    constexpr int32_t argc = 3;
    const char *argv[]     = {static_cast<const char *>("cluon-msc"), static_cast<const char *>("--out=ABC0.out"), static_cast<const char *>("ABC0.odvd")};

    const char *input = R"(
message testdata.MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
    int32 attribute6 [ default = -10000, id = 6 ];
)";
    std::string messageSpecification(input);

    std::fstream odvd("ABC0.odvd", std::ios::out);
    odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
    odvd.close();

    REQUIRE(1 == cluon_msc(argc, const_cast<char **>(argv)));

    UNLINK("ABC0.odvd");
    UNLINK("ABC0.out");
}

TEST_CASE("Test no conversion with valid ODVD.") {
    UNLINK("ABC1.odvd");
    UNLINK("ABC1.out");

    constexpr int32_t argc = 3;
    const char *argv[]     = {static_cast<const char *>("cluon-msc"), static_cast<const char *>("--out=ABC1.out"), static_cast<const char *>("ABC1.odvd")};

    const char *input = R"(
message testdata.MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
    int32 attribute6 [ default = -10000, id = 6 ];
    uint64 attribute7 [ default = 12345, id = 7 ];
    int64 attribute8 [ default = -12345, id = 8 ];
    float attribute9 [ default = -1.2345, id = 9 ];
    double attribute10 [ default = -10.2345, id = 10 ];
    string attribute11 [ default = "Hello World!", id = 11 ];
}
)";
    std::string messageSpecification(input);

    std::fstream odvd("ABC1.odvd", std::ios::out);
    odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
    odvd.close();

    REQUIRE(0 == cluon_msc(argc, const_cast<char **>(argv)));

    std::fstream out("ABC1.out", std::ios::in);
    REQUIRE(out.good());

    std::stringstream sstrOutput;
    std::string output;
    while (getline(out, output)) { sstrOutput << output << std::endl; }
    out.close();
    output = sstrOutput.str();

    const char *expectedOutput = R"(
)";

    REQUIRE(output == std::string(expectedOutput));

    UNLINK("ABC1.odvd");
    UNLINK("ABC1.out");
}

TEST_CASE("Test Proto conversion with valid ODVD.") {
    UNLINK("ABC2.odvd");
    UNLINK("ABC2.proto");

    constexpr int32_t argc = 4;
    const char *argv[]     = {static_cast<const char *>("cluon-msc"),
                          static_cast<const char *>("--proto"),
                          static_cast<const char *>("--out=ABC2.proto"),
                          static_cast<const char *>("ABC2.odvd")};

    const char *input = R"(
message testdata.MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
    int32 attribute6 [ default = -10000, id = 6 ];
    uint64 attribute7 [ default = 12345, id = 7 ];
    int64 attribute8 [ default = -12345, id = 8 ];
    float attribute9 [ default = -1.2345, id = 9 ];
    double attribute10 [ default = -10.2345, id = 10 ];
    string attribute11 [ default = "Hello World!", id = 11 ];
}
)";
    std::string messageSpecification(input);

    std::fstream odvd("ABC2.odvd", std::ios::out);
    odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
    odvd.close();

    REQUIRE(0 == cluon_msc(argc, const_cast<char **>(argv)));

    std::fstream out("ABC2.proto", std::ios::in);
    REQUIRE(out.good());

    std::stringstream sstrOutput;
    std::string output;
    while (getline(out, output)) { sstrOutput << output << std::endl; }
    out.close();
    output = sstrOutput.str();

    const char *expectedOutput = R"(
//
// THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
//


// This line is only needed when using Google Protobuf 3.
syntax = "proto2";


// Message identifier: 30005.
message testdata_MyTestMessage5 {
    
    optional uint32 attribute1 = 1;
    
    optional sint32 attribute2 = 2;
    
    optional uint32 attribute3 = 3;
    
    optional sint32 attribute4 = 4;
    
    optional uint32 attribute5 = 5;
    
    optional sint32 attribute6 = 6;
    
    optional uint64 attribute7 = 7;
    
    optional sint64 attribute8 = 8;
    
    optional float attribute9 = 9;
    
    optional double attribute10 = 10;
    
    optional string attribute11 = 11;
    
}

)";

    REQUIRE(output == std::string(expectedOutput));

    UNLINK("ABC2.odvd");
    UNLINK("ABC2.proto");
}

TEST_CASE("Test .hpp conversion with valid ODVD.") {
    UNLINK("ABC3.odvd");
    UNLINK("ABC3.hpp");

    constexpr int32_t argc = 4;
    const char *argv[]     = {static_cast<const char *>("cluon-msc"),
                          static_cast<const char *>("--cpp-headers"),
                          static_cast<const char *>("--out=ABC3.hpp"),
                          static_cast<const char *>("ABC3.odvd")};

    const char *input = R"(
message testdata.MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
    int32 attribute6 [ default = -10000, id = 6 ];
    uint64 attribute7 [ default = 12345, id = 7 ];
    int64 attribute8 [ default = -12345, id = 8 ];
    float attribute9 [ default = -1.2345, id = 9 ];
    double attribute10 [ default = -10.2345, id = 10 ];
    string attribute11 [ default = "Hello World!", id = 11 ];
}
)";
    std::string messageSpecification(input);

    std::fstream odvd("ABC3.odvd", std::ios::out);
    odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
    odvd.close();

    REQUIRE(0 == cluon_msc(argc, const_cast<char **>(argv)));

    std::fstream out("ABC3.hpp", std::ios::in);
    REQUIRE(out.good());

    std::stringstream sstrOutput;
    std::string output;
    while (getline(out, output)) { sstrOutput << output << std::endl; }
    out.close();
    output = sstrOutput.str();

    const char *expectedOutput = R"(
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


#ifndef TESTDATA_MYTESTMESSAGE5_HPP
#define TESTDATA_MYTESTMESSAGE5_HPP

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
namespace testdata {
using namespace std::string_literals; // NOLINT
class LIB_API MyTestMessage5 {
    public:
        MyTestMessage5() = default;
        MyTestMessage5(const MyTestMessage5&) = default;
        MyTestMessage5& operator=(const MyTestMessage5&) = default;
        MyTestMessage5(MyTestMessage5&&) = default; // NOLINT
        MyTestMessage5& operator=(MyTestMessage5&&) = default; // NOLINT
        ~MyTestMessage5() = default;

    public:
        static int32_t ID();
        static const std::string ShortName();
        static const std::string LongName();
        
        MyTestMessage5& attribute1(const uint8_t &v) noexcept;
        uint8_t attribute1() const noexcept;
        
        MyTestMessage5& attribute2(const int8_t &v) noexcept;
        int8_t attribute2() const noexcept;
        
        MyTestMessage5& attribute3(const uint16_t &v) noexcept;
        uint16_t attribute3() const noexcept;
        
        MyTestMessage5& attribute4(const int16_t &v) noexcept;
        int16_t attribute4() const noexcept;
        
        MyTestMessage5& attribute5(const uint32_t &v) noexcept;
        uint32_t attribute5() const noexcept;
        
        MyTestMessage5& attribute6(const int32_t &v) noexcept;
        int32_t attribute6() const noexcept;
        
        MyTestMessage5& attribute7(const uint64_t &v) noexcept;
        uint64_t attribute7() const noexcept;
        
        MyTestMessage5& attribute8(const int64_t &v) noexcept;
        int64_t attribute8() const noexcept;
        
        MyTestMessage5& attribute9(const float &v) noexcept;
        float attribute9() const noexcept;
        
        MyTestMessage5& attribute10(const double &v) noexcept;
        double attribute10() const noexcept;
        
        MyTestMessage5& attribute11(const std::string &v) noexcept;
        std::string attribute11() const noexcept;
        

        template<class Visitor>
        void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            
            doVisit(1, std::move("uint8_t"s), std::move("attribute1"s), m_attribute1, visitor);
            
            doVisit(2, std::move("int8_t"s), std::move("attribute2"s), m_attribute2, visitor);
            
            doVisit(3, std::move("uint16_t"s), std::move("attribute3"s), m_attribute3, visitor);
            
            doVisit(4, std::move("int16_t"s), std::move("attribute4"s), m_attribute4, visitor);
            
            doVisit(5, std::move("uint32_t"s), std::move("attribute5"s), m_attribute5, visitor);
            
            doVisit(6, std::move("int32_t"s), std::move("attribute6"s), m_attribute6, visitor);
            
            doVisit(7, std::move("uint64_t"s), std::move("attribute7"s), m_attribute7, visitor);
            
            doVisit(8, std::move("int64_t"s), std::move("attribute8"s), m_attribute8, visitor);
            
            doVisit(9, std::move("float"s), std::move("attribute9"s), m_attribute9, visitor);
            
            doVisit(10, std::move("double"s), std::move("attribute10"s), m_attribute10, visitor);
            
            doVisit(11, std::move("std::string"s), std::move("attribute11"s), m_attribute11, visitor);
            
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            
            doTripletForwardVisit(1, std::move("uint8_t"s), std::move("attribute1"s), m_attribute1, preVisit, visit, postVisit);
            
            doTripletForwardVisit(2, std::move("int8_t"s), std::move("attribute2"s), m_attribute2, preVisit, visit, postVisit);
            
            doTripletForwardVisit(3, std::move("uint16_t"s), std::move("attribute3"s), m_attribute3, preVisit, visit, postVisit);
            
            doTripletForwardVisit(4, std::move("int16_t"s), std::move("attribute4"s), m_attribute4, preVisit, visit, postVisit);
            
            doTripletForwardVisit(5, std::move("uint32_t"s), std::move("attribute5"s), m_attribute5, preVisit, visit, postVisit);
            
            doTripletForwardVisit(6, std::move("int32_t"s), std::move("attribute6"s), m_attribute6, preVisit, visit, postVisit);
            
            doTripletForwardVisit(7, std::move("uint64_t"s), std::move("attribute7"s), m_attribute7, preVisit, visit, postVisit);
            
            doTripletForwardVisit(8, std::move("int64_t"s), std::move("attribute8"s), m_attribute8, preVisit, visit, postVisit);
            
            doTripletForwardVisit(9, std::move("float"s), std::move("attribute9"s), m_attribute9, preVisit, visit, postVisit);
            
            doTripletForwardVisit(10, std::move("double"s), std::move("attribute10"s), m_attribute10, preVisit, visit, postVisit);
            
            doTripletForwardVisit(11, std::move("std::string"s), std::move("attribute11"s), m_attribute11, preVisit, visit, postVisit);
            
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        
        uint8_t m_attribute1{ 1 }; // field identifier = 1.
        
        int8_t m_attribute2{ -1 }; // field identifier = 2.
        
        uint16_t m_attribute3{ 100 }; // field identifier = 3.
        
        int16_t m_attribute4{ -100 }; // field identifier = 4.
        
        uint32_t m_attribute5{ 10000 }; // field identifier = 5.
        
        int32_t m_attribute6{ -10000 }; // field identifier = 6.
        
        uint64_t m_attribute7{ 12345 }; // field identifier = 7.
        
        int64_t m_attribute8{ -12345 }; // field identifier = 8.
        
        float m_attribute9{ -1.2345f }; // field identifier = 9.
        
        double m_attribute10{ -10.2345 }; // field identifier = 10.
        
        std::string m_attribute11{ "Hello World!"s }; // field identifier = 11.
        
};
}

template<>
struct isVisitable<testdata::MyTestMessage5> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<testdata::MyTestMessage5> {
    static const bool value = true;
};
#endif

)";

    REQUIRE(output == std::string(expectedOutput));

    UNLINK("ABC3.odvd");
    UNLINK("ABC3.proto");
}

TEST_CASE("Test .cpp conversion with valid ODVD and no additional include header.") {
    UNLINK("ABC4.odvd");
    UNLINK("ABC4.cpp");

    constexpr int32_t argc = 4;
    const char *argv[]     = {static_cast<const char *>("cluon-msc"),
                          static_cast<const char *>("--cpp-sources"),
                          static_cast<const char *>("--out=ABC4.cpp"),
                          static_cast<const char *>("ABC4.odvd")};

    const char *input = R"(
message testdata.MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
    int32 attribute6 [ default = -10000, id = 6 ];
    uint64 attribute7 [ default = 12345, id = 7 ];
    int64 attribute8 [ default = -12345, id = 8 ];
    float attribute9 [ default = -1.2345, id = 9 ];
    double attribute10 [ default = -10.2345, id = 10 ];
    string attribute11 [ default = "Hello World!", id = 11 ];
}
)";
    std::string messageSpecification(input);

    std::fstream odvd("ABC4.odvd", std::ios::out);
    odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
    odvd.close();

    REQUIRE(0 == cluon_msc(argc, const_cast<char **>(argv)));

    std::fstream out("ABC4.cpp", std::ios::in);
    REQUIRE(out.good());

    std::stringstream sstrOutput;
    std::string output;
    while (getline(out, output)) { sstrOutput << output << std::endl; }
    out.close();
    output = sstrOutput.str();

    const char *expectedOutput = R"(
/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace testdata {

int32_t MyTestMessage5::ID() {
    return 30005;
}

const std::string MyTestMessage5::ShortName() {
    return "MyTestMessage5";
}
const std::string MyTestMessage5::LongName() {
    return "testdata.MyTestMessage5";
}

MyTestMessage5& MyTestMessage5::attribute1(const uint8_t &v) noexcept {
    m_attribute1 = v;
    return *this;
}
uint8_t MyTestMessage5::attribute1() const noexcept {
    return m_attribute1;
}

MyTestMessage5& MyTestMessage5::attribute2(const int8_t &v) noexcept {
    m_attribute2 = v;
    return *this;
}
int8_t MyTestMessage5::attribute2() const noexcept {
    return m_attribute2;
}

MyTestMessage5& MyTestMessage5::attribute3(const uint16_t &v) noexcept {
    m_attribute3 = v;
    return *this;
}
uint16_t MyTestMessage5::attribute3() const noexcept {
    return m_attribute3;
}

MyTestMessage5& MyTestMessage5::attribute4(const int16_t &v) noexcept {
    m_attribute4 = v;
    return *this;
}
int16_t MyTestMessage5::attribute4() const noexcept {
    return m_attribute4;
}

MyTestMessage5& MyTestMessage5::attribute5(const uint32_t &v) noexcept {
    m_attribute5 = v;
    return *this;
}
uint32_t MyTestMessage5::attribute5() const noexcept {
    return m_attribute5;
}

MyTestMessage5& MyTestMessage5::attribute6(const int32_t &v) noexcept {
    m_attribute6 = v;
    return *this;
}
int32_t MyTestMessage5::attribute6() const noexcept {
    return m_attribute6;
}

MyTestMessage5& MyTestMessage5::attribute7(const uint64_t &v) noexcept {
    m_attribute7 = v;
    return *this;
}
uint64_t MyTestMessage5::attribute7() const noexcept {
    return m_attribute7;
}

MyTestMessage5& MyTestMessage5::attribute8(const int64_t &v) noexcept {
    m_attribute8 = v;
    return *this;
}
int64_t MyTestMessage5::attribute8() const noexcept {
    return m_attribute8;
}

MyTestMessage5& MyTestMessage5::attribute9(const float &v) noexcept {
    m_attribute9 = v;
    return *this;
}
float MyTestMessage5::attribute9() const noexcept {
    return m_attribute9;
}

MyTestMessage5& MyTestMessage5::attribute10(const double &v) noexcept {
    m_attribute10 = v;
    return *this;
}
double MyTestMessage5::attribute10() const noexcept {
    return m_attribute10;
}

MyTestMessage5& MyTestMessage5::attribute11(const std::string &v) noexcept {
    m_attribute11 = v;
    return *this;
}
std::string MyTestMessage5::attribute11() const noexcept {
    return m_attribute11;
}

}

)";

    REQUIRE(output == std::string(expectedOutput));

    UNLINK("ABC4.odvd");
    UNLINK("ABC4.cpp");
}

TEST_CASE("Test .cpp conversion with valid ODVD and additional include header.") {
    UNLINK("ABC5.odvd");
    UNLINK("ABC5.cpp");

    constexpr int32_t argc = 5;
    const char *argv[]     = {static_cast<const char *>("cluon-msc"),
                          static_cast<const char *>("--cpp-sources"),
                          static_cast<const char *>("--cpp-add-include-file=ABC5.hpp"),
                          static_cast<const char *>("--out=ABC5.cpp"),
                          static_cast<const char *>("ABC5.odvd")};

    const char *input = R"(
message testdata.MyTestMessage5 [id = 30005] {
    uint8 attribute1 [ default = 1, id = 1 ];
    int8 attribute2 [ default = -1, id = 2 ];
    uint16 attribute3 [ default = 100, id = 3 ];
    int16 attribute4 [ default = -100, id = 4 ];
    uint32 attribute5 [ default = 10000, id = 5 ];
    int32 attribute6 [ default = -10000, id = 6 ];
    uint64 attribute7 [ default = 12345, id = 7 ];
    int64 attribute8 [ default = -12345, id = 8 ];
    float attribute9 [ default = -1.2345, id = 9 ];
    double attribute10 [ default = -10.2345, id = 10 ];
    string attribute11 [ default = "Hello World!", id = 11 ];
}
)";
    std::string messageSpecification(input);

    std::fstream odvd("ABC5.odvd", std::ios::out);
    odvd.write(messageSpecification.c_str(), static_cast<std::streamsize>(messageSpecification.size()));
    odvd.close();

    REQUIRE(0 == cluon_msc(argc, const_cast<char **>(argv)));

    std::fstream out("ABC5.cpp", std::ios::in);
    REQUIRE(out.good());

    std::stringstream sstrOutput;
    std::string output;
    while (getline(out, output)) { sstrOutput << output << std::endl; }
    out.close();
    output = sstrOutput.str();

    const char *expectedOutput = R"(#include <ABC5.hpp>

/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */
namespace testdata {

int32_t MyTestMessage5::ID() {
    return 30005;
}

const std::string MyTestMessage5::ShortName() {
    return "MyTestMessage5";
}
const std::string MyTestMessage5::LongName() {
    return "testdata.MyTestMessage5";
}

MyTestMessage5& MyTestMessage5::attribute1(const uint8_t &v) noexcept {
    m_attribute1 = v;
    return *this;
}
uint8_t MyTestMessage5::attribute1() const noexcept {
    return m_attribute1;
}

MyTestMessage5& MyTestMessage5::attribute2(const int8_t &v) noexcept {
    m_attribute2 = v;
    return *this;
}
int8_t MyTestMessage5::attribute2() const noexcept {
    return m_attribute2;
}

MyTestMessage5& MyTestMessage5::attribute3(const uint16_t &v) noexcept {
    m_attribute3 = v;
    return *this;
}
uint16_t MyTestMessage5::attribute3() const noexcept {
    return m_attribute3;
}

MyTestMessage5& MyTestMessage5::attribute4(const int16_t &v) noexcept {
    m_attribute4 = v;
    return *this;
}
int16_t MyTestMessage5::attribute4() const noexcept {
    return m_attribute4;
}

MyTestMessage5& MyTestMessage5::attribute5(const uint32_t &v) noexcept {
    m_attribute5 = v;
    return *this;
}
uint32_t MyTestMessage5::attribute5() const noexcept {
    return m_attribute5;
}

MyTestMessage5& MyTestMessage5::attribute6(const int32_t &v) noexcept {
    m_attribute6 = v;
    return *this;
}
int32_t MyTestMessage5::attribute6() const noexcept {
    return m_attribute6;
}

MyTestMessage5& MyTestMessage5::attribute7(const uint64_t &v) noexcept {
    m_attribute7 = v;
    return *this;
}
uint64_t MyTestMessage5::attribute7() const noexcept {
    return m_attribute7;
}

MyTestMessage5& MyTestMessage5::attribute8(const int64_t &v) noexcept {
    m_attribute8 = v;
    return *this;
}
int64_t MyTestMessage5::attribute8() const noexcept {
    return m_attribute8;
}

MyTestMessage5& MyTestMessage5::attribute9(const float &v) noexcept {
    m_attribute9 = v;
    return *this;
}
float MyTestMessage5::attribute9() const noexcept {
    return m_attribute9;
}

MyTestMessage5& MyTestMessage5::attribute10(const double &v) noexcept {
    m_attribute10 = v;
    return *this;
}
double MyTestMessage5::attribute10() const noexcept {
    return m_attribute10;
}

MyTestMessage5& MyTestMessage5::attribute11(const std::string &v) noexcept {
    m_attribute11 = v;
    return *this;
}
std::string MyTestMessage5::attribute11() const noexcept {
    return m_attribute11;
}

}

)";

    REQUIRE(output == std::string(expectedOutput));

    UNLINK("ABC5.odvd");
    UNLINK("ABC5.cpp");
}
