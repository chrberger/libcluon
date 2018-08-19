# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

################################################################################
# Get source files to run cppcheck.
file(GLOB_RECURSE ALL_SOURCES *.[ch]pp *.cc *.[ch])

# Adding cppcheck target if executable is found
find_program(CPPCHECK "cppcheck")
if(CPPCHECK)
  add_custom_target(
    cppcheck
    COMMAND /usr/bin/cppcheck
        --enable=warning,style,performance,portability,information,unusedFunction
        --template=gcc
        --inline-suppr
        --library=gnu.cfg
        --library=posix.cfg
        --std=posix --std=c++11 --quiet
    ${ALL_SOURCES}
    )
endif()

