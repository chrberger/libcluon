# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

find_program(iwyu_path NAMES include-what-you-use iwyu)
if(NOT iwyu_path)
    message("Could not find the program include-what-you-use")
endif()

