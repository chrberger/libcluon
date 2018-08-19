# Copyright (C) 2017-2018  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# If not found, use the system's search paths.
FIND_PATH(CLUON_INCLUDE_DIR cluon.hpp
            NAMES   cluon
            PATHS   /usr/include
                    /usr/local/include)

FIND_LIBRARY(CLUON_LIBRARY
            NAMES   cluon cluon-static
            PATHS   /usr/lib
                    /usr/lib64
                    /usr/local/lib
                    /usr/local/lib64)

FIND_LIBRARY(CLUON_STATIC_LIBRARY
            NAMES   cluon-static
            PATHS   /usr/lib
                    /usr/lib64
                    /usr/local/lib
                    /usr/local/lib64)

IF("${CLUON_INCLUDE_DIR}" STREQUAL "")
    MESSAGE(FATAL_ERROR "Could not find libcluon.")
ELSE()
    SET (FOUND_CLUON 1)
ENDIF()

###########################################################################
# Artifacts based on libcluon need to link against a threading library.
FIND_PACKAGE (Threads REQUIRED)

if(UNIX)
    if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")

        if(NOT LIBRT_FOUND)

            IF(${CMAKE_C_COMPILER} MATCHES "arm")
                # We are on ARM.
                find_path(LIBRT_INCLUDE_DIR
                    NAMES
                        time.h
                    PATHS
                        ${LIBRTDIR}/include/
                )

                find_file(
                    LIBRT_LIBRARIES librt.a
                    PATHS
                        ${LIBRTDIR}/lib/
                        /usr/lib/arm-linux-gnueabihf/
                        /usr/lib/arm-linux-gnueabi/
                )
                set (LIBRT_DYNAMIC "Using static library.")

                if (NOT LIBRT_LIBRARIES)
                    find_library(
                        LIBRT_LIBRARIES rt
                        PATHS
                            ${LIBRTDIR}/lib/
                            /usr/lib/arm-linux-gnueabihf/
                            /usr/lib/arm-linux-gnueabi/
                    )
                    set (LIBRT_DYNAMIC "Using dynamic library.")
                endif (NOT LIBRT_LIBRARIES)
            ELSE()
                IF("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
                    # We are on x86_64.
                    find_path(LIBRT_INCLUDE_DIR
                        NAMES
                            time.h
                        PATHS
                            ${LIBRTDIR}/include/
                    )

                    find_file(
                        LIBRT_LIBRARIES librt.a
                        PATHS
                            ${LIBRTDIR}/lib/
                            /usr/lib/x86_64-linux-gnu/
                            /usr/local/lib64/
                            /usr/lib64/
                            /usr/lib/
                    )
                    set (LIBRT_DYNAMIC "Using static library.")

                    if (NOT LIBRT_LIBRARIES)
                        find_library(
                            LIBRT_LIBRARIES rt
                            PATHS
                                ${LIBRTDIR}/lib/
                                /usr/lib/x86_64-linux-gnu/
                                /usr/local/lib64/
                                /usr/lib64/
                                /usr/lib/
                        )
                        set (LIBRT_DYNAMIC "Using dynamic library.")
                    endif (NOT LIBRT_LIBRARIES)
                ELSE()
                    # We are on x86.
                    find_path(LIBRT_INCLUDE_DIR
                        NAMES
                            time.h
                        PATHS
                            ${LIBRTDIR}/include/
                    )

                    find_file(
                        LIBRT_LIBRARIES librt.a
                        PATHS
                            ${LIBRTDIR}/lib/
                            /usr/lib/i386-linux-gnu/
                            /usr/local/lib/
                            /usr/lib/
                    )
                    set (LIBRT_DYNAMIC "Using static library.")

                    if (NOT LIBRT_LIBRARIES)
                        find_library(
                            LIBRT_LIBRARIES rt
                            PATHS
                                ${LIBRTDIR}/lib/
                                /usr/lib/i386-linux-gnu/
                                /usr/local/lib/
                                /usr/lib/
                        )
                        set (LIBRT_DYNAMIC "Using dynamic library.")
                    endif (NOT LIBRT_LIBRARIES)
                ENDIF()
            ENDIF()

            if (LIBRT_INCLUDE_DIR AND LIBRT_LIBRARIES)
                set (LIBRT_FOUND TRUE)
            endif (LIBRT_INCLUDE_DIR AND LIBRT_LIBRARIES)

            if (LIBRT_FOUND)
                message(STATUS "Found librt: ${LIBRT_INCLUDE_DIR}, ${LIBRT_LIBRARIES} ${LIBRT_DYNAMIC}")
            else (LIBRT_FOUND)
                if (Librt_FIND_REQUIRED)
                    message (FATAL_ERROR "Could not find librt, try to setup LIBRT_PREFIX accordingly")
                endif (Librt_FIND_REQUIRED)
            endif (LIBRT_FOUND)

        endif (NOT LIBRT_FOUND)
    endif()
endif()

###########################################################################
# Set linking libraries.
SET(CLUON_LIBRARIES ${CLUON_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${LIBRT_LIBRARIES})
SET(CLUON_STATIC_LIBRARIES ${CLUON_STATIC_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${LIBRT_LIBRARIES})
SET(CLUON_INCLUDE_DIRS ${CLUON_INCLUDE_DIR} {${LIBRT_INCLUDE_DIR}})

###########################################################################
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libcluon DEFAULT_MSG CLUON_LIBRARY CLUON_STATIC_LIBRARY CLUON_INCLUDE_DIR)
MARK_AS_ADVANCED(CLUON_INCLUDE_DIR CLUON_LIBRARY CLUON_STATIC_LIBRARY)

