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

class ImageTile : public MediaTile
{
private:
    int file_size_;
 //   boost::filesystem::path filepath_;
    
    bool encode_;
    std::mutex mtx_;
    std::mutex it_mtx_;
    
    std::unique_ptr<std::vector<uint8_t>> image_;

public:
    ImageTile(const std::string& tile_id, std::vector<cv::Mat>&& frames, int mw, int mh);
    ~ImageTile();
    
    virtual void encode();
    virtual int fileSize();
    virtual std::shared_ptr<std::vector<uint8_t>> getSegment(uint32_t segment_no);
};