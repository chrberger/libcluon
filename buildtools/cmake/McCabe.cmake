# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

################################################################################
# Get source files to run pmccabe.
file(GLOB_RECURSE ALL_SOURCES *.[ch]pp *.cc *.[ch])

# Adding pmccabe target if executable is found
find_program(PMCCABE "pmccabe")
if(PMCCABE)
  add_custom_target(
    pmccabe
    COMMAND /usr/bin/pmccabe
    -v
    ${ALL_SOURCES}
    )
endif()

