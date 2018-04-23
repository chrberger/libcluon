# Copyright (C) 2017  Christian Berger
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

    set(GENERAL_BUILD_FLAGS "${GENERAL_BUILD_FLAGS} \
        -D_XOPEN_SOURCE=700 \
        -fstack-protector \
        -pipe ")

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} \
         ${GENERAL_BUILD_FLAGS} \
        -D_FORTIFY_SOURCE=2 \
        -O2 \
        -fomit-frame-pointer ")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
         ${GENERAL_BUILD_FLAGS} \
        -D_FORTIFY_SOURCE=2 \
        -O2 \
        -fomit-frame-pointer ")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        ${GENERAL_BUILD_FLAGS} \
        ${CXX_WARNING_FLAGS}")

    # Remove symbols from libraries.
    if(NOT("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin"))
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ")
        set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -s ")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s ")
    endif()
endif()

