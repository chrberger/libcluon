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

enable_testing()

################################################################################
# Get testsuites to run test cases.
file(GLOB testsuites "${CMAKE_CURRENT_SOURCE_DIR}/testsuites/Test*.cpp")

if(ENABLE_COVERAGE)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        find_program(LLVM_COV "llvm-cov")
        find_program(LLVM_PROFDATA "llvm-profdata")
        if(NOT LLVM_PROFDATA)
            if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.8))
                find_program(LLVM_PROFDATA "llvm-profdata-3.8")
            endif()
            if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.0))
                find_program(LLVM_PROFDATA "llvm-profdata-4.0")
            endif()
        endif()
    endif()
    if(LLVM_COV AND LLVM_PROFDATA)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    endif()
endif()

if(ENABLE_PERFORMANCE)
    find_program(VALGRIND "valgrind")
endif()

# Create runner.cpp to speed up compilation for Catch xUnit framework.
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/runner.cpp "#define CATCH_CONFIG_MAIN\n#include \"catch.hpp\"\n")
# Create temporary library to force CMake compile Catch runner.
add_library(${PROJECT_NAME}runnerlib STATIC ${CMAKE_CURRENT_BINARY_DIR}/runner.cpp)

# Iterate trough all testsuites.
foreach(testsuite ${testsuites})
    # Generate a short name for the testrunner.
    string(REPLACE "/" ";" testsuite-list ${testsuite})
    list(LENGTH testsuite-list len)
    math(EXPR lastItem "${len}-1")
    list(GET testsuite-list "${lastItem}" testsuite-shortname)

    # Add executable for the testrunner.
    add_executable(${testsuite-shortname}-Runner ${testsuite})
    target_link_libraries(${testsuite-shortname}-Runner ${PROJECT_NAME}runnerlib ${LIBRARIES} ${ADDITIONAL_LIBRARIES_FOR_TESTING})

    # Add testrunner.
    if(NOT ENABLE_PERFORMANCE)
#        add_test(NAME ${testsuite}-TestSuite-Runner COMMAND ${testsuite-shortname}-Runner --reporter junit -o ${testsuite-shortname}-Runner.xml)
        add_test(NAME ${testsuite}-TestSuite-Runner COMMAND ${testsuite-shortname}-Runner)
    endif()

    # Enable code coverage calculation.
    if(ENABLE_COVERAGE AND LLVM_COV AND LLVM_PROFDATA)
        add_test(NAME ${testsuite}-TestSuite-RunnerPrepareCoverage COMMAND ${LLVM_PROFDATA} merge -o ${testsuite-shortname}-Runner.profdata ${testsuite-shortname}-Runner.profraw)
        set_tests_properties(${testsuite}-TestSuite-Runner PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${testsuite-shortname}-Runner.profraw")

        add_custom_command(OUTPUT ReportCoverage_${testsuite-shortname}.output
                           COMMAND llvm-cov report ${testsuite-shortname}-Runner -instr-profile=${testsuite-shortname}-Runner.profdata)
        add_custom_target(ReportCoverage_${testsuite-shortname} DEPENDS ReportCoverage_${testsuite-shortname}.output)

        add_custom_command(OUTPUT ShowCoverage_${testsuite-shortname}.output
                           COMMAND  llvm-cov show ${testsuite-shortname}-Runner -use-color -instr-profile=${testsuite-shortname}-Runner.profdata)
        add_custom_target(ShowCoverage_${testsuite-shortname} DEPENDS ShowCoverage_${testsuite-shortname}.output)
    endif()

    # Enable performance checking.
    if(ENABLE_PERFORMANCE AND VALGRIND)
        add_test(NAME ${testsuite}-TestSuite-Runner COMMAND valgrind --tool=callgrind --dump-instr=yes --compress-pos=no --compress-strings=no --callgrind-out-file=${testsuite-shortname}-Runner.callgrind ${CMAKE_CURRENT_BINARY_DIR}/${testsuite-shortname}-Runner)
    endif()

    set_tests_properties(${testsuite}-TestSuite-Runner PROPERTIES TIMEOUT 3000)
endforeach()

