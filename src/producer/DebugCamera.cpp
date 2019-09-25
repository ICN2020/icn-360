/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#include <opencv2/highgui/highgui.hpp>
#include "ParametersProducer.h"
#include "CameraMetadata.h"
#include "CapturedFrame.h"
#include "DebugCamera.h"

DebugCamera::DebugCamera()
    : image_(),
      frame_(),
      base_time_()
{
    image_ = cv::imread(ParametersProducer::debugFilename(), cv::IMREAD_COLOR);
    frame_ = std::make_shared<CapturedFrame>(image_.rows, image_.cols);
    base_time_ = std::chrono::system_clock::now();
    fiv_ = ParametersProducer::videoSegmentSize();
    fiv_ = fiv_ > 0 ? fiv_ : DEFAULT_FRAME_INTERVAL_TIME;

}


DebugCamera::~DebugCamera()
{

}


void
DebugCamera::getCameraMetadata(CameraMetadata& metadata)
{
    metadata.width = image_.cols;
    metadata.height = image_.rows;
    metadata.fiv = fiv_;
    metadata.tile[0] = ParametersProducer::matrixNumX();
    metadata.tile[1] = ParametersProducer::matrixNumY();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - base_time_);
    metadata.seq = diff.count() / fiv_;
}


std::shared_ptr<CapturedFrame>
DebugCamera::getCapturedFrame(uint32_t sequenceNo){
    return frame_;
}
