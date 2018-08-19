# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

################################################################################
# Get source files to run flint++.
file(GLOB_RECURSE ALL_SOURCES *.[ch]pp *.cc *.[ch])

# Adding Flint++ target if executable is found
find_program(FLINT_FORMAT "flint++")
if(FLINT_FORMAT)
  add_custom_target(
    flint++
    COMMAND /usr/bin/flint++
    -l3 -v
    ${ALL_SOURCES}
    )
endif()

