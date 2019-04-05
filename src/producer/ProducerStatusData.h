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

struct ProducerStatusData
{
    int consumers;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(consumers));
    }
};