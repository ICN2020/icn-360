/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once

// Naming
constexpr char APP_PREFIX[]         = "/icn2020.org/theta";
constexpr char META_DATA_PATH[]     = "/icn2020.org/theta/metadata";
constexpr char IMG_DATA_PATH[]      = "/icn2020.org/theta/frames";
constexpr char META_DATA_SUFFIX[]   = "metadata";
constexpr char IMG_DATA_SUFFIX[]    = "frames";
constexpr char STATUS_DATA_SUFFIX[] = "status";
constexpr char LOW_DATA_PREFIX[]    = "f:transcoding";


constexpr int NAME_SEGMENT_LOCATION         = -1;
constexpr int NAME_COORDINATE_Y_LOCATION    = -2;
constexpr int NAME_COORDINATE_X_LOCATION    = -3;
constexpr int NAME_SEQUENCE_LOCATION        = -4;
constexpr int NAME_QUALITY_LOCATION         = -5;