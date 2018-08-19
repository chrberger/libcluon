/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// This test for a compiler definition is necessary to preserve single-file, header-only compability.
#ifndef HAVE_CLUON_REC2CSV
#include "cluon-rec2csv.hpp"
#endif

#include <cstdint>

int32_t main(int32_t argc, char **argv) {
    return cluon_rec2csv(argc, argv);
}
