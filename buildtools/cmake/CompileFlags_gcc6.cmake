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

# Warning flags based on and generated from: https://raw.githubusercontent.com/Barro/compiler-warnings/master/gcc/warnings-gcc-6.txt
set(GCC6_CXX_WARNING_FLAGS " \
    -pedantic \
    -pedantic-errors \
    -Werror \
    -Wall \
    -Wabi \
    -Waddress \
    -Wno-error=aggregate-return \
    -Wall \
    -Warray-bounds \
    -Warray-bounds=2 \
    -Wattributes \
    -Wbool-compare \
    -Wbuiltin-macro-redefined \
    -Wno-error=c++11-compat \
    -Wc++14-compat \
    -Wcast-align \
    -Wcast-qual \
    -Wchar-subscripts \
    -Wchkp \
    -Wclobbered \
    -Wcomment \
    -Wcomments \
    -Wconditionally-supported \
    -Wno-error=conversion \
    -Wconversion-null \
    -Wcoverage-mismatch \
    -Wcpp \
    -Wctor-dtor-privacy \
    -Wdate-time \
    -Wdelete-incomplete \
    -Wdelete-non-virtual-dtor \
    -Wdeprecated \
    -Wdeprecated-declarations \
    -Wdisabled-optimization \
    -Wdiv-by-zero \
    -Wdouble-promotion \
    -Wno-error=effc++ \
    -Wempty-body \
    -Wendif-labels \
    -Wenum-compare \
    -Werror-implicit-function-declaration \
    -Wextra \
    -Wfloat-conversion \
    -Wfloat-equal \
    -Wformat \
    -Wformat-contains-nul \
    -Wformat-extra-args \
    -Wformat-nonliteral \
    -Wformat-security \
    -Wformat-signedness \
    -Wformat-y2k \
    -Wformat-zero-length \
    -Wformat=1 \
    -Wfree-nonheap-object \
    -Wignored-qualifiers \
    -Wimport \
    -Winherited-variadic-ctor \
    -Winit-self \
    -Wno-error=inline \
    -Wint-to-pointer-cast \
    -Winvalid-memory-model \
    -Winvalid-offsetof \
    -Winvalid-pch \
    -Wliteral-suffix \
    -Wlogical-not-parentheses \
    -Wlogical-op \
    -Wno-error=long-long \
    -Wmain \
    -Wmaybe-uninitialized \
    -Wmemset-transposed-args \
    -Wmissing-braces \
    -Wmissing-declarations \
    -Wmissing-field-initializers \
    -Wmissing-format-attribute \
    -Wsuggest-attribute=format \
    -Wno-error=missing-include-dirs \
    -Wmissing-noreturn \
    -Wsuggest-attribute=noreturn \
    -Wmultichar \
    -Wnarrowing \
    -Wnoexcept \
    -Wnon-template-friend \
    -Wnon-virtual-dtor \
    -Wnonnull \
    -Wnormalized=nfc \
    -Wodr \
    -Wold-style-cast \
    -Wopenmp-simd \
    -Woverflow \
    -Woverlength-strings \
    -Woverloaded-virtual \
    -Wpacked \
    -Wpacked-bitfield-compat \
    -Wparentheses \
    -Wpedantic \
    -Wpmf-conversions \
    -Wpointer-arith \
    -Wpragmas \
    -Wpsabi \
    -Wredundant-decls \
    -Wreorder \
    -Wreturn-local-addr \
    -Wreturn-type \
    -Wsequence-point \
    -Wshadow \
    -Wshift-count-negative \
    -Wshift-count-overflow \
    -Wsign-compare \
    -Wno-error=sign-conversion \
    -Wsized-deallocation \
    -Wsizeof-array-argument \
    -Wsizeof-pointer-memaccess \
    -Wstack-protector \
    -Wno-error=strict-aliasing \
    -Wstrict-aliasing=3 \
    -Wstrict-null-sentinel \
    -Wstrict-overflow \
    -Wstrict-overflow=5 \
    -Wno-error=suggest-attribute=const \
    -Wno-error=suggest-attribute=format \
    -Wno-error=suggest-attribute=noreturn \
    -Wno-error=suggest-attribute=pure \
    -Wno-error=suggest-final-methods \
    -Wno-error=suggest-final-types \
    -Wno-error=suggest-override \
    -Wswitch \
    -Wswitch-bool \
    -Wswitch-default \
    -Wswitch-enum \
    -Wsync-nand \
    -Wsynth \
    -Wtrampolines \
    -Wtrigraphs \
    -Wno-error=type-limits \
    -Wundef \
    -Wuninitialized \
    -Wunknown-pragmas \
    -Wno-error=unreachable-code \
    -Wno-error=unsafe-loop-optimizations \
    -Wunused \
    -Wunused-but-set-parameter \
    -Wunused-but-set-variable \
    -Wunused-function \
    -Wunused-label \
    -Wunused-local-typedefs \
    -Wunused-parameter \
    -Wunused-result \
    -Wunused-value \
    -Wunused-variable \
    -Wno-error=useless-cast \
    -Wvarargs \
    -Wvariadic-macros \
    -Wvector-operation-performance \
    -Wvirtual-move-assign \
    -Wvla \
    -Wvolatile-register-var \
    -Wwrite-strings \
    -Wno-error=zero-as-null-pointer-constant")

