/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once
#include <chrono>
#include <opencv2/core/core.hpp>
#include "Camera.h"


class DebugCamera : public Camera
{
private:
    cv::Mat image_;
    std::shared_ptr<CapturedFrame> frame_;
    std::chrono::system_clock::time_point base_time_;
    int fiv_;

public:
    DebugCamera();
    virtual ~DebugCamera();
  
    std::shared_ptr<CapturedFrame> getCapturedFrame(uint32_t sequenceNo);
    void getCameraMetadata(CameraMetadata& metadata);
};
