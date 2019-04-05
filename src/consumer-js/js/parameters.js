/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

Parameters = {
    NAME_PREFIX:                "/icn2020.org/theta",
    MATA_DATA_PREFIX:           "/icn2020.org/theta/metadata",
    IMG_DATA_PREFIX:            "/icn2020.org/theta/frames",
    METADATA_UPDATE_INTERVAL:   5000, // 5sec
    DOWNLOAD_WAIT_INTERVAL:     250, 
    DOWNLOAD_PIPELINES:         10,
    FIXED_WINDOW_SIZE:          1,
    INTEREST_TIMEOUT:           500,
    HTTP_MODE:                  false,
    SHOW_GRID:                  false,
    EDGE_MARK:                  true,
    VR_MODE:                    false,
    DEVICE_ORIENTATION:         true,
};