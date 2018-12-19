# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.8))
        include(CompileFlags_clang3.8)
        set(CXX_WARNING_FLAGS ${CLANG38_CXX_WARNING_FLAGS})
    endif ()
    if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.0))
        include(CompileFlags_clang4.0)
        set(CXX_WARNING_FLAGS ${CLANG40_CXX_WARNING_FLAGS})
    endif ()
    if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.0))
        include(CompileFlags_clang5.0)
        set(CXX_WARNING_FLAGS ${CLANG50_CXX_WARNING_FLAGS})
    endif ()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 6.0))
        include(CompileFlags_gcc6)
        set(CXX_WARNING_FLAGS ${GCC6_CXX_WARNING_FLAGS})
    endif ()
    if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 7.0))
        include(CompileFlags_gcc7)
        set(CXX_WARNING_FLAGS ${GCC7_CXX_WARNING_FLAGS})
    endif ()
    if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 8.2))
        include(CompileFlags_gcc8.2)
        set(CXX_WARNING_FLAGS ${GCC82_CXX_WARNING_FLAGS})
    endif ()
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    set(GENERAL_BUILD_FLAGS "-D_DARWIN_C_SOURCE=1 ")
endif()

if(WIN32)
    set(GENERAL_BUILD_FLAGS "${GENERAL_BUILD_FLAGS} \
        /O2 ")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        ${GENERAL_BUILD_FLAGS} ")
else()
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
        set(GENERAL_BUILD_FLAGS "${GENERAL_BUILD_FLAGS} \
            -D__BSD_VISIBLE=1 ")
    endif()

    if (   (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "NetBSD"))
       AND (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD")) )
        set(GENERAL_BUILD_FLAGS "${GENERAL_BUILD_FLAGS} \
            -D_XOPEN_SOURCE=700 ")
        set(GENERAL_BUILD_FLAGS_RELEASE "${GENERAL_BUILD_FLAGS} \
            -O2 \
            -fomit-frame-pointer \
            -D_FORTIFY_SOURCE=2 ")
    endif()

    set(GENERAL_BUILD_FLAGS "${GENERAL_BUILD_FLAGS} \
        -fstack-protector-strong \
        -pipe ")

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} \
         ${GENERAL_BUILD_FLAGS_RELEASE} ")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
         ${GENERAL_BUILD_FLAGS_RELEASE} ")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        ${GENERAL_BUILD_FLAGS} \
        ${CXX_WARNING_FLAGS}")

    # Remove symbols from libraries.
    if (   (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin"))
       AND (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD")) )
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ")
        set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -s ")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s ")
    endif()
endif()

