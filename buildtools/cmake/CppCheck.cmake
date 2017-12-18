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

