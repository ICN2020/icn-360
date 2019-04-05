/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once
#include <string>
#include <set>
#include <tuple>
#include <thread>
#include <mutex>
#include <memory>
#include <deque>
#include <opencv2/highgui/highgui.hpp>
#include "CameraMetadata.h"
#include "CapturedFrame.h"
#include "Camera.h"


class CvCamera : public Camera
{
private:
    cv::VideoCapture camera_;
    cv::Mat frame_;
    uint32_t width_, height_;
    std::thread thr_;
    uint32_t frame_interval_;
    bool run_;

    std::deque<std::shared_ptr<CapturedFrame>> frame_queue_;
    std::mutex mtx_;

public:
    CvCamera();
    virtual ~CvCamera();
  
    void start();
    void stop();

    bool open(int device, uint32_t width = 0, uint32_t height = 0, uint32_t fps = 0);
    bool open(const std::string &file);
    
    inline uint32_t width() const { return width_; }
    inline uint32_t height() const { return height_; }
    
    std::shared_ptr<CapturedFrame> getCapturedFrame(uint32_t sequenceNo);
    void getCamereaMetadata(CameraMetadata& metadata);

private:
    virtual bool initCamera(uint32_t init_width, uint32_t init_height, uint32_t init_fps);
    virtual void capture();

    CvCamera(const CvCamera &c);
    CvCamera &operator=(const CvCamera &w);
};
