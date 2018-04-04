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

