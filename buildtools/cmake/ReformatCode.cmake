# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

################################################################################
# Get source files to run clang-format.
file(GLOB_RECURSE ALL_SOURCES include/*.[ch]pp include/*.cc include/*.[ch]
                              src/*.[ch]pp src/*.cc src/*.[ch]
                              testsuites/*.[ch]pp testsuites/*.cc testsuites/*.[ch])

# Adding clang-format target if executable is found
find_program(CLANG_FORMAT NAMES clang-format clang-format-6.0)
if(CLANG_FORMAT)
  add_custom_target(
    reformat-code
    COMMAND ${CLANG_FORMAT}
    -i
    ${ALL_SOURCES}
    )
endif()

