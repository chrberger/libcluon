# You may redistribute this program and/or modify it under the terms of
# the GNU General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
