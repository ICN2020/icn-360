/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once
#include <cereal/cereal.hpp>

struct CameraMetadata
{
    int width;
    int height;
    int fiv;
    int fps;
    int tile[2];
    int seq;
    int mui; // Metadata Update Interval [ms]
    std::string mime;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(width), CEREAL_NVP(height),
        CEREAL_NVP(fiv), CEREAL_NVP(fps),
        CEREAL_NVP(tile), CEREAL_NVP(seq), CEREAL_NVP(mui),
        CEREAL_NVP(mime));
    }
};