/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#include "ParametersProducer.h"
#include "ConsumerCounter.h"


ConsumerCounter::ConsumerCounter()
    : counter_(0),
      consumer_num_(0)
{
    start_time_ = std::chrono::system_clock::now();
}


void
ConsumerCounter::increase()
{
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();
    if(elapsed > METADATA_UPDATE_INTERVAL){
        consumer_num_ = counter_.load();
        counter_.exchange(0);
        start_time_ = std::chrono::system_clock::now();
    }
    int expected = counter_.load();
    int desired;
    do {
        desired = expected + 1;
    } while (!counter_.compare_exchange_weak(expected, desired));
}


int
ConsumerCounter::get()
{
    return consumer_num_;
}
