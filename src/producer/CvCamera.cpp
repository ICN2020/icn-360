/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#include <iostream>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <algorithm>
#include "ParametersProducer.h"
#include "CvCamera.h"


CvCamera::CvCamera()
    : camera_(),
      frame_(),
      frame_interval_(),
      run_(false)
{
}

CvCamera::~CvCamera()
{
    stop();
}

bool
CvCamera::initCamera(uint32_t init_width, uint32_t init_height, uint32_t init_fps)
{
    if (init_width > 0) camera_.set(CV_CAP_PROP_FRAME_WIDTH, init_width);
    if (init_height > 0) camera_.set(CV_CAP_PROP_FRAME_HEIGHT, init_height);
    if (init_fps > 0) camera_.set(CV_CAP_PROP_FPS, init_fps);
    
    if (camera_.grab())
    {
        width_ = static_cast<uint32_t>(camera_.get(CV_CAP_PROP_FRAME_WIDTH));
        height_ = static_cast<uint32_t>(camera_.get(CV_CAP_PROP_FRAME_HEIGHT));
        if (width_ == 0) width_ = init_width;
        if (height_ == 0) height_ = init_height;
        
        frame_.release();
        camera_.retrieve(frame_, 3);

        uint32_t fps = static_cast<uint32_t>(camera_.get(CV_CAP_PROP_FPS));
        frame_interval_ = (fps > 0 && fps < 1000) ? static_cast<uint32_t>(1000 / fps) : DEFAULT_FRAME_INTERVAL_TIME;
        
        return true;
    }
    return false;
}


void
CvCamera::capture()
{
    auto frame_count = static_cast<uint32_t>(camera_.get(CV_CAP_PROP_FRAME_COUNT));
    bool is_loop = (frame_count > 0) &&  ParametersProducer::isLoop();

    std::shared_ptr<CapturedFrame> captured_frame;
    while (run_) {
        if (camera_.grab())
        {
            frame_.release();
            camera_.retrieve(frame_);
            
            if(captured_frame.get() == nullptr ||
                captured_frame->elapsedTime() > ParametersProducer::videoSegmentSize()){
                if(captured_frame.get() != nullptr){
                    std::lock_guard<std::mutex> locker(mtx_);
                    while(frame_queue_.size() >= FRAME_BUFFER_LENGTH){
                        frame_queue_.pop_back();
                    }
                    frame_queue_.push_front(captured_frame);
                }
                captured_frame = std::make_shared<CapturedFrame>(width_, height_);
            }
            captured_frame->append(frame_);
        } else {
            if(is_loop){
                //if(static_cast<uint32_t>(camera_.get(CV_CAP_PROP_POS_FRAMES)) == frame_count){
                    camera_.set(CV_CAP_PROP_POS_FRAMES, 0);
                //}
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1L));
        }
    }
}


void
CvCamera::start()
{
    run_ = true;
    thr_ = std::thread([this](){ this->capture(); });
}

void
CvCamera::stop()
{
    if (run_)
    {
        run_ = false;
        thr_.join();
    }
}

bool
CvCamera::open(int device, uint32_t width, uint32_t height, uint32_t fps)
{
    camera_.open(device);
    if (camera_.isOpened() && initCamera(width, height, fps))
        return true;

    return false;
}


bool
CvCamera::open(const std::string &file)
{
    camera_.open(file);
    if (camera_.isOpened() && initCamera(0, 0, 0)) return true;
    
    return false;
}

void
CvCamera::getCamereaMetadata(CameraMetadata& metadata)
{
    metadata.width = width_;
    metadata.height = height_;
    metadata.fiv = ParametersProducer::videoSegmentSize();
    metadata.tile[0] = ParametersProducer::matrixNumX();
    metadata.tile[1] = ParametersProducer::matrixNumY();
    metadata.seq = CapturedFrame::currentSequenceNo();    
    metadata.mime = ParametersProducer::mediaCodec().mime();
    metadata.fps = ParametersProducer::mediaCodec().isImage() ? metadata.fiv : frame_interval_;    
}


std::shared_ptr<CapturedFrame>
CvCamera::getCapturedFrame(uint32_t sequenceNo){
    std::lock_guard<std::mutex> locker(mtx_);
    auto  frame = std::find_if(frame_queue_.begin(), frame_queue_.end(), [&sequenceNo](const std::shared_ptr<CapturedFrame> i){
        return i->sequenceNo() == sequenceNo;
    });
    return (frame == frame_queue_.end()) ? nullptr : *frame;
}