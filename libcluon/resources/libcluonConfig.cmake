# Copyright (C) 2018  Christian Berger
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

IF("${CLUON_INCLUDE_DIR}" STREQUAL "")
    MESSAGE(FATAL_ERROR "Could not find libcluon.")
ELSE()
    SET (FOUND_CLUON 1)
ENDIF()

###########################################################################
# Artifacts based on libcluon need to link against a threading library.
FIND_PACKAGE (Threads REQUIRED)

###########################################################################
# Set linking libraries.
SET(CLUON_LIBRARIES ${CLUON_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
SET(CLUON_INCLUDE_DIRS ${CLUON_INCLUDE_DIR})

###########################################################################
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libcluon DEFAULT_MSG CLUON_LIBRARY CLUON_INCLUDE_DIR)
MARK_AS_ADVANCED(CLUON_INCLUDE_DIR CLUON_LIBRARY)

