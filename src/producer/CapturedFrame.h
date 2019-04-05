/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <map>
#include <opencv2/highgui/highgui.hpp>
#include "MediaTile.h"

class CapturedFrame {
private:
    static uint32_t next_sequence_;

    std::mutex mtx_;
    std::chrono::system_clock::time_point startTime_;
    std::vector<const cv::Mat> frame_vector_;

    uint32_t sequence_; 
    size_t width_;
    size_t height_;

    std::unordered_map<std::string, std::shared_ptr<MediaTile>> tiles_;
    
public:
    CapturedFrame(size_t width, size_t height);
    virtual ~CapturedFrame();

    void append(const cv::Mat mat);

    int elapsedTime(){ return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime_).count(); };
    size_t tileNum(){ return frame_vector_.size(); };
    
    uint32_t sequenceNo() const { return sequence_; };
    static uint32_t currentSequenceNo() { return (next_sequence_ - 1); };

    void writeTiles();
    std::shared_ptr<MediaTile> getMediaTile(int x, int y, bool low_quality);

protected:
    std::shared_ptr<MediaTile> createNewVideoTile(int x, int y, bool low_quality);
    std::shared_ptr<MediaTile> createNewImageTile(int x, int y, bool low_quality);
};
