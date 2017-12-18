#!/bin/bash

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

# Script for building.
cat <<EOF > /tmp/build.sh
#!/bin/bash

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

export PATH=/usr/lib/ccache:\$PATH
export CCACHE_DIR=/opt/ccache

# Build command.
COMMAND=$1
SOURCE_FOLDER=$2

BUILDER=make
HAS_NINJA=\$(which ninja)
HAS_UPX=\$(which upx)
CMAKE_PREFIX=""
if [ "x\$HAS_NINJA" != "x" ]; then
    CMAKE_PREFIX="-G Ninja"
    BUILDER=ninja
fi

if [ -x /usr/lib/ccache/cc -a -x /usr/lib/ccache/c++ ]; then
    CMAKE_PREFIX="\$CMAKE_PREFIX \
                  -D CMAKE_C_COMPILER=/usr/lib/ccache/cc \
                  -D CMAKE_CXX_COMPILER=/usr/lib/ccache/c++"
fi
if [ -x /usr/lib/ccache/bin/cc -a -x /usr/lib/ccache/bin/c++ ]; then
    CMAKE_PREFIX="\$CMAKE_PREFIX \
                  -D CMAKE_C_COMPILER=/usr/lib/ccache/bin/cc \
                  -D CMAKE_CXX_COMPILER=/usr/lib/ccache/bin/c++"
fi
if [ -x /usr/lib/ccache/clang -a -x /usr/lib/ccache/clang++ ]; then
    CMAKE_PREFIX="\$CMAKE_PREFIX \
                  -D CMAKE_C_COMPILER=/usr/lib/ccache/clang \
                  -D CMAKE_CXX_COMPILER=/usr/lib/ccache/clang++"
fi

CMAKE_PARAMETERS_RELEASE="\$CMAKE_PREFIX \
                  -D CMAKE_BUILD_TYPE=Release"
CMAKE_PARAMETERS_DEBUG="\$CMAKE_PREFIX \
                  -D CMAKE_BUILD_TYPE=Debug"
CMAKE_PARAMETERS_CODE_COVERAGE="\$CMAKE_PARAMETERS_DEBUG \
                  -D ENABLE_COVERAGE=1"
CMAKE_PARAMETERS_PERFORMANCE="\$CMAKE_PARAMETERS_DEBUG \
                  -D ENABLE_PERFORMANCE=1"

cd /opt/build
if [ "\$COMMAND" == "compile" \
    -o "\$COMMAND" == "test" \
    -o "\$COMMAND" == "install" \
    -o "\$COMMAND" == "reformat-code" ]; then
    if [ ! -d build ]; then
        mkdir build && cd build && \
        cmake \$CMAKE_PARAMETERS_RELEASE \
            -D CMAKE_INSTALL_PREFIX=/opt/install \
            /opt/sources/\$SOURCE_FOLDER && \
        cd ..
    fi
    cd build
    if [ "\$COMMAND" == "compile" ]; then
        \$BUILDER
    fi
    if [ "\$COMMAND" == "test" ]; then
        \$BUILDER && CTEST_OUTPUT_ON_FAILURE=1 \$BUILDER test
    fi
    if [ "\$COMMAND" == "install" ]; then
        \$BUILDER && \$BUILDER install
        if [ "x\$HAS_UPX" != "x" ]; then
            for i in \$(find /opt/install -type f -perm -755); do
                \$HAS_UPX -4 \$i
            done
        fi
    fi
    if [ "\$COMMAND" == "reformat-code" ]; then
        \$BUILDER reformat-code
    fi
fi
if [ "\$COMMAND" == "memory-leaks" ]; then
    if [ ! -d memory-leaks ]; then
        mkdir memory-leaks
    fi
    cd memory-leaks
        cmake \$CMAKE_PARAMETERS_DEBUG \
        -D CMAKE_CXX_FLAGS="-fsanitize=address" \
        /opt/sources/\$SOURCE_FOLDER && \
    \$BUILDER && ctest -V
fi
if [ "\$COMMAND" == "code-coverage" ]; then
    if [ ! -d code-coverage ]; then
        mkdir code-coverage
    fi
    cd code-coverage
    cmake \$CMAKE_PARAMETERS_CODE_COVERAGE \
        /opt/sources/\$SOURCE_FOLDER && \
    \$BUILDER && CTEST_OUTPUT_ON_FAILURE=1 \$BUILDER test && \
        for i in \$(\$BUILDER -t targets | grep "ReportCoverage" | cut -f1 -d":"); do
            echo \$i && \$BUILDER \$i
        done
fi
if [ "\$COMMAND" == "show-coverage" ]; then
    if [ -d code-coverage ]; then
        cd code-coverage
        for i in \$(\$BUILDER -t targets | grep "ShowCoverage" | cut -f1 -d":"); do
            echo \$i && \$BUILDER \$i
        done
    fi
fi
if [ "\$COMMAND" == "static-code-analysis" ]; then
    if [ ! -d static-code-analysis ]; then
        mkdir static-code-analysis
    fi
    cd static-code-analysis && rm -f report
    STATIC_CODE_ANALYSIS_PWD=\$PWD
    scan-build cmake -G "Unix Makefiles" \
        -D CMAKE_BUILD_TYPE=Debug \
        /opt/sources/\$SOURCE_FOLDER 2>/dev/null 1>/dev/null && \
    echo "Running scan-build."
    echo "=================================================================" 1> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "Report: scan-build" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "-----------------------------------------------------------------" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    scan-build -o /opt/build/report -v -enable-checker alpha.core,alpha.cplusplus,alpha.osx,alpha.security,alpha.unix,alpha.valist,core,cplusplus,deadcode,nullability,optin,security,unix make 2>&1 | grep "^/opt" | grep -v "should add these lines:" | grep -v "/thirdparty/" | grep -v "should remove these lines:" | grep -v "buildtools/xUnit/catch.hpp" | grep -v "Werror=literal-suffix" 1>> \$STATIC_CODE_ANALYSIS_PWD/report

    # Run clang-tidy.
    echo "Running clang-tidy."
    echo "=================================================================" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "Report: clang-tidy" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "-----------------------------------------------------------------" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    OLD_PWD=\$PWD
    mkdir -p clang-tidy && cd clang-tidy && cmake \$CMAKE_PARAMETERS_DEBUG \
                                -DCMAKE_CXX_CLANG_TIDY:STRING="clang-tidy;-checks=boost-*,bugprone-*,cert-*,cppcoreguidelines-*,clang-analyzer-*,google-*,hicpp-*,llvm-*,misc-*,modernize-*,mpi-*,performance-*,readability-*,-google-readability-namespace-comments,-llvm-namespace-comment,-readability-redundant-member-init,-llvm-include-order,-readability-implicit-bool-cast,-google-readability-function-size" \
                                /opt/sources/\$SOURCE_FOLDER 2>&1 > /dev/null && \$BUILDER 2>&1 | grep "^/opt" | grep -v "should add these lines:" | grep -v "should remove these lines:" | grep -v "/thirdparty/" | grep -v "catch.hpp" | grep -v "Wunreachable-code-return" 1>> \$STATIC_CODE_ANALYSIS_PWD/report
    cd \$OLD_PWD

    echo "Running flawfinder."
    echo "=================================================================" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "Report: flawfinder" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "-----------------------------------------------------------------" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    flawfinder -QS /opt/sources/\$SOURCE_FOLDER 2>&1 | grep "^/opt" | grep -v "/thirdparty/" 1>> \$STATIC_CODE_ANALYSIS_PWD/report

    # Run oclint.
    echo "Running oclint."
    echo "=================================================================" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "Report: oclint" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "-----------------------------------------------------------------" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    OLD_PWD=\$PWD
    mkdir -p oclint && cd oclint && cmake \$CMAKE_PARAMETERS_DEBUG \
                                -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                                /opt/sources/\$SOURCE_FOLDER 2>&1 > /dev/null && ln -sf \$PWD/compile_commands.json /opt/sources/\$SOURCE_FOLDER
    for i in \$(find /opt/sources/\$SOURCE_FOLDER -type f -name "*.cpp"); do
        /usr/local/oclint-0.12/bin/oclint -p /opt/sources/\$SOURCE_FOLDER -enable-global-analysis -enable-clang-static-analyzer \$i 2>&1 | grep "^/opt" | grep -v "/thirdparty/" 1>> \$STATIC_CODE_ANALYSIS_PWD/report
    done && rm -f /opt/sources/\$SOURCE_FOLDER/compile_commands.json
    cd \$OLD_PWD

    # Run vera++ but ignore:
    #T011: closing curly bracket not in the same line or column
    #L001: trailing whitespace
    #L004: line is longer than 100 characters
    #L005: too many consecutive empty lines
    #T009: comma should not be preceded by whitespace
    #T012: negation operator used in its short form
    echo "Running vera++."
    echo "=================================================================" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "Report: vera++" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "-----------------------------------------------------------------" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    for i in \$(find /opt/sources/\$SOURCE_FOLDER -type f -name "*.cpp"); do
        vera++ -s \$i 2>&1 | grep "^/opt" | grep -v "/thirdparty/" | grep -v "T009:" | grep -v "T011:" | grep -v "T012:" | grep -v "L001:" | grep -v "L004:" | grep -v "L005:" 1>> \$STATIC_CODE_ANALYSIS_PWD/report
    done

    echo "Running cppcheck."
    echo "=================================================================" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "Report: cppcheck" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "-----------------------------------------------------------------" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    make cppcheck 2>&1 | grep "^/opt" | grep -v "/thirdparty/" | grep -v "The scope of the variable 'EXPECTED_" 1>> \$STATIC_CODE_ANALYSIS_PWD/report

    echo "Running flint++."
    echo "=================================================================" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "Report: flint++" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "-----------------------------------------------------------------" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    make flint++ 2>&1 | grep " /opt" | grep -v "/thirdparty/" | grep -v "MessageParser.cpp" | sed -e "s/\[.*]\ //g" 1>> \$STATIC_CODE_ANALYSIS_PWD/report

    echo "Running pmccabe."
    echo "=================================================================" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "Report: pmccabe" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    echo "-----------------------------------------------------------------" 1>> \$STATIC_CODE_ANALYSIS_PWD/report && \
    make pmccabe 2>/dev/null | grep -v "/thirdparty/" 2>/dev/null 1>> \$STATIC_CODE_ANALYSIS_PWD/report

    cat \$STATIC_CODE_ANALYSIS_PWD/report
fi
if [ "\$COMMAND" == "performance" ]; then
    if [ ! -d performance ]; then
        mkdir performance
    fi
    cd performance
        cmake \$CMAKE_PARAMETERS_PERFORMANCE \
        /opt/sources/\$SOURCE_FOLDER && \
    \$BUILDER && ctest -V
fi
EOF

chmod 755 /tmp/build.sh
chown $USER_FOR_BUILDING /tmp/build.sh

su -m `getent passwd $USER_FOR_BUILDING|cut -f1 -d":"` -c /tmp/build.sh

chown -R $USER_FOR_BUILDING /opt/build

