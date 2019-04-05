/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <string>
#include <boost/filesystem.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "MediaTile.h"

class MediaTile
{
public:
    static std::string tileId(uint32_t seq, int x, int y, bool low_quarity);

protected:
    std::vector<cv::Mat> frames_;
    int width_;
    int height_;
    
public:
    MediaTile(const std::string& tile_id, std::vector<cv::Mat>&& frames, int mw, int mh);
    virtual ~MediaTile();
    
    virtual void encode() = 0;
    virtual int fileSize() = 0;
    virtual std::shared_ptr<std::vector<uint8_t>> getSegment(uint32_t segment_no) = 0;
    
    uint32_t finalSegmentNo();
};