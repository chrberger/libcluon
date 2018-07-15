# Copyright (C) 2017-2018  Christian Berger
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.2)

project(cluon)

################################################################################
# Set the search path for .cmake files.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../buildtools/cmake" ${CMAKE_MODULE_PATH})

################################################################################
# Include default CMake modules.
include(CompileFlags)
include(CppCheck)
include(Flint++)
include(IncludeWhatYouUse)
include(McCabe)
include(ReformatCode)

################################################################################
# Find threads library.
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(LIBRARIES ${LIBRARIES} Threads::Threads)

if( ("${WEB}" STREQUAL "") OR ("${WEB}" STREQUAL "0") )
    if(UNIX)
        if ( (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
           AND (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD") )
            find_package(LibRT REQUIRED)
            set(LIBRARIES ${LIBRARIES} ${LIBRT_LIBRARIES})
            include_directories(SYSTEM ${LIBRT_INCLUDE_DIR})
        endif()
    endif()
endif()

################################################################################
# Check whether code for JavaScript is generated.
if("${WEB}" STREQUAL "1")
    message("Compiling for JavaScript.")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++14 --bind -s NO_EXIT_RUNTIME=1 -s MODULARIZE=1 --closure 1 -s EXPORT_NAME=\"'libcluon'\" -O3 --memory-init-file 0 --js-opts 1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 --bind -s NO_EXIT_RUNTIME=1 -s MODULARIZE=1 --closure 1 -s EXPORT_NAME=\"'libcluon'\" -O3 --memory-init-file 0 --js-opts 1")
    set(CMAKE_EXECUTABLE_SUFFIX .js)
    set(CMAKE_EXE_LINKER_FLAGS "")
endif()

################################################################################
# Define include directories:
# Add third party folders.
include_directories(SYSTEM thirdparty)
# Configure this project's targets.
include_directories(include)
include_directories(tools)

################################################################################
# Before we can build libcluon, we need to build a small helper tool to compile
# message specifications.
set(CLUON-MSC cluon-msc)
set(CLUON-MSC-LIBCLUON-SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/MetaMessage.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/MetaMessageToCPPTransformator.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/MetaMessageToProtoTransformator.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/MessageParser.cpp)
add_library(${CLUON-MSC}-objects OBJECT ${CLUON-MSC-LIBCLUON-SOURCES})
set_target_properties(${CLUON-MSC}-objects PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(${CLUON-MSC} ${CMAKE_CURRENT_SOURCE_DIR}/tools/cluon-msc.cpp $<TARGET_OBJECTS:${CLUON-MSC}-objects>)
target_link_libraries(${CLUON-MSC} Threads::Threads)

###############################################################################
# Add sources for the actual library.
file(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Remove the entries that have been already built for ${CLUON-MSC}.
FOREACH(entry ${CLUON-MSC-LIBCLUON-SOURCES})
    list(REMOVE_ITEM thisproject-sources ${entry})
ENDFOREACH()

add_library(${PROJECT_NAME}-objects OBJECT ${thisproject-sources})
set_target_properties(${PROJECT_NAME}-objects PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} SHARED        $<TARGET_OBJECTS:${PROJECT_NAME}-objects> $<TARGET_OBJECTS:${CLUON-MSC}-objects>)
add_library(${PROJECT_NAME}-static STATIC $<TARGET_OBJECTS:${PROJECT_NAME}-objects> $<TARGET_OBJECTS:${CLUON-MSC}-objects>)

# Define resulting libraries to link.
set(LIBRARIES ${PROJECT_NAME}-static ${LIBRARIES})

# These binaries are only built for the non-Web version.
if("${WEB}" STREQUAL "")
    set(CLUON-OD4TOSTDOUT cluon-OD4toStdout)
    add_executable(${CLUON-OD4TOSTDOUT} ${CMAKE_CURRENT_SOURCE_DIR}/tools/${CLUON-OD4TOSTDOUT}.cpp)
    target_link_libraries(${CLUON-OD4TOSTDOUT} ${LIBRARIES})

    set(CLUON-OD4TOJSON cluon-OD4toJSON)
    add_executable(${CLUON-OD4TOJSON} ${CMAKE_CURRENT_SOURCE_DIR}/tools/${CLUON-OD4TOJSON}.cpp)
    target_link_libraries(${CLUON-OD4TOJSON} ${LIBRARIES})

    set(CLUON-LCMTOJSON cluon-LCMtoJSON)
    add_executable(${CLUON-LCMTOJSON} ${CMAKE_CURRENT_SOURCE_DIR}/tools/${CLUON-LCMTOJSON}.cpp)
    target_link_libraries(${CLUON-LCMTOJSON} ${LIBRARIES})

    set(CLUON-LIVEFEED cluon-livefeed)
    add_executable(${CLUON-LIVEFEED} ${CMAKE_CURRENT_SOURCE_DIR}/tools/${CLUON-LIVEFEED}.cpp)
    target_link_libraries(${CLUON-LIVEFEED} ${LIBRARIES})

    set(CLUON-REC2CSV cluon-rec2csv)
    add_executable(${CLUON-REC2CSV} ${CMAKE_CURRENT_SOURCE_DIR}/tools/${CLUON-REC2CSV}.cpp)
    target_link_libraries(${CLUON-REC2CSV} ${LIBRARIES})

    set(CLUON-REPLAY cluon-replay)
    add_executable(${CLUON-REPLAY} ${CMAKE_CURRENT_SOURCE_DIR}/tools/${CLUON-REPLAY}.cpp)
    target_link_libraries(${CLUON-REPLAY} ${LIBRARIES})
endif()

# The target for the JavaScript interface.
if("${WEB}" STREQUAL "1")
    add_executable(libcluon ${CMAKE_CURRENT_SOURCE_DIR}/tools/libcluon.cpp)
    target_link_libraries(libcluon ${LIBRARIES})
endif()


# This command will generate an additional single source file that we can add to the build tree.
if("${WEB}" STREQUAL "")
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/include/cluon/cluonDataStructures.hpp
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/cluon
                   COMMAND ${CLUON-MSC} --cpp --out=${CMAKE_BINARY_DIR}/include/cluon/cluonDataStructures.hpp ${CMAKE_CURRENT_SOURCE_DIR}/resources/cluonDataStructures.odvd
                   COMMAND ${CLUON-MSC} --proto --out=${CMAKE_BINARY_DIR}/cluonDataStructures.proto ${CMAKE_CURRENT_SOURCE_DIR}/resources/cluonDataStructures.odvd
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/cluonDataStructures.odvd
                           ${CLUON-MSC})

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/include/cluon/cluonTestDataStructures.hpp
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/cluon
                   COMMAND ${CLUON-MSC} --cpp --out=${CMAKE_BINARY_DIR}/include/cluon/cluonTestDataStructures.hpp ${CMAKE_CURRENT_SOURCE_DIR}/resources/cluonTestDataStructures.odvd
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/cluonTestDataStructures.odvd
                           ${CLUON-MSC})
else()
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/include/cluon/cluonDataStructures.hpp
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/cluon
                   COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/resources/cluonDataStructures.odvd | docker run --rm -i chrberger/cluon:latest ${CLUON-MSC} --cpp --out=/dev/stdout /dev/stdin > ${CMAKE_BINARY_DIR}/include/cluon/cluonDataStructures.hpp
                   COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/resources/cluonDataStructures.odvd | docker run --rm -i chrberger/cluon:latest ${CLUON-MSC} --proto --out=/dev/stdout /dev/stdin > ${CMAKE_BINARY_DIR}/cluonDataStructures.proto
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/cluonDataStructures.odvd)
endif()

# Add dependency the generate the header files.
add_custom_target(generate_cluonDataStructures_hpp DEPENDS ${CMAKE_BINARY_DIR}/include/cluon/cluonDataStructures.hpp)
add_dependencies(${PROJECT_NAME}-objects generate_cluonDataStructures_hpp)
if("${WEB}" STREQUAL "")
    add_custom_target(generate_cluonTestDataStructures_hpp DEPENDS ${CMAKE_BINARY_DIR}/include/cluon/cluonTestDataStructures.hpp)
    add_dependencies(${PROJECT_NAME}-objects generate_cluonTestDataStructures_hpp)
endif()

include_directories(${CMAKE_BINARY_DIR}/include)

###############################################################################
# Add executables from examples folder for the non-Web version.
if("${WEB}" STREQUAL "")
    file(GLOB_RECURSE thisproject-example-sources "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp")
    foreach(example ${thisproject-example-sources})
        string(REPLACE "/" ";" entry-list ${example})
        list(LENGTH entry-list len)
        math(EXPR lastItem "${len}-1")
        list(GET entry-list "${lastItem}" example-shortname)

        string(REPLACE ".cpp" "" example-shortname-binary ${example-shortname})
        add_executable(${example-shortname-binary} ${example})
        target_link_libraries(${example-shortname-binary} ${LIBRARIES})
    endforeach()
endif()

###############################################################################
# Install this project for the non-Web version.
if("${WEB}" STREQUAL "")
    install(TARGETS ${PROJECT_NAME}-static DESTINATION lib COMPONENT lib${PROJECT_NAME})
    install(TARGETS ${PROJECT_NAME}        DESTINATION lib COMPONENT lib${PROJECT_NAME})
    install(TARGETS ${CLUON-MSC}           DESTINATION bin COMPONENT lib${PROJECT_NAME})
    install(TARGETS ${CLUON-OD4TOSTDOUT}   DESTINATION bin COMPONENT lib${PROJECT_NAME})
    install(TARGETS ${CLUON-OD4TOJSON}     DESTINATION bin COMPONENT lib${PROJECT_NAME})
    install(TARGETS ${CLUON-LCMTOJSON}     DESTINATION bin COMPONENT lib${PROJECT_NAME})
    install(TARGETS ${CLUON-LIVEFEED}      DESTINATION bin COMPONENT lib${PROJECT_NAME})
    install(TARGETS ${CLUON-REC2CSV}       DESTINATION bin COMPONENT lib${PROJECT_NAME})
    install(TARGETS ${CLUON-REPLAY}        DESTINATION bin COMPONENT lib${PROJECT_NAME})
    # Install header files.
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include COMPONENT lib${PROJECT_NAME})
    install(FILES "${CMAKE_BINARY_DIR}/include/cluon/cluonDataStructures.hpp" DESTINATION include/cluon COMPONENT lib${PROJECT_NAME})
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cluon/any/any.hpp" DESTINATION include/cluon/any/ COMPONENT lib${PROJECT_NAME})
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cluon/stringtoolbox.hpp" DESTINATION include/cluon/ COMPONENT lib${PROJECT_NAME})
    # Install message specification files.
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/cluonDataStructures.odvd" DESTINATION share/${PROJECT_NAME} COMPONENT lib${PROJECT_NAME})
    install(FILES "${CMAKE_BINARY_DIR}/cluonDataStructures.proto" DESTINATION share/${PROJECT_NAME} COMPONENT lib${PROJECT_NAME})
    # Install Findlibcluon.cmake.
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/libcluonConfig.cmake" DESTINATION lib/${CMAKE_LIBRARY_ARCHITECTURE}/cmake/lib${PROJECT_NAME} COMPONENT lib${PROJECT_NAME})
endif()

###############################################################################
# Check superfluous include directives.
#if(NOT "${iwyu_path}" STREQUAL "iwyu_path-NOTFOUND")
#    set_property(TARGET ${PROJECT_NAME}-objects PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
#endif()

################################################################################
# Include Catch unit test framework (MUST follow this project's targets as
# ${LIBRARIES} is used in the cmake file for catch.hpp).
include_directories(../buildtools/xUnit)
if("${WEB}" STREQUAL "")
    include(xUnitCatch)
endif()

