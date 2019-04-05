/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once
#include <atomic>
#include <chrono>

class ConsumerCounter 
{
private:
    std::atomic<int> counter_;
    int consumer_num_;
    std::chrono::system_clock::time_point start_time_;

public:
    ConsumerCounter();

    void increase();
    int get();
};
