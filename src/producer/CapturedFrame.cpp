/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#include <iostream>
#include <iterator>
#include <memory>
#include <boost/filesystem.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc.hpp>
#include "Constants.h"
#include "ParametersProducer.h"
#include "VideoTile.h"
#include "ImageTile.h"
#include "CapturedFrame.h"


namespace fs = boost::filesystem;

uint32_t CapturedFrame::next_sequence_ = 0;

CapturedFrame::CapturedFrame(size_t width, size_t height)
    : width_(width), height_(height)
{
    sequence_ = next_sequence_++;
    startTime_ = std::chrono::system_clock::now();
}

CapturedFrame::~CapturedFrame(){

}

void
CapturedFrame::append(const cv::Mat mat){
    frame_vector_.push_back(mat);
}

void
CapturedFrame::writeTiles(){
}


std::shared_ptr<MediaTile>
CapturedFrame::getMediaTile(int x, int y, bool low_quality){
    int matrix_x = ParametersProducer::matrixNumX();
    int matrix_y = ParametersProducer::matrixNumY();
    bool is_image = ParametersProducer::mediaCodec().isImage();

    if (x < 0 || y < 0 || x >= matrix_x || y >= matrix_y) return nullptr;

    std::lock_guard<std::mutex> locker(mtx_);

    std::shared_ptr<MediaTile> tile;
    if (low_quality){
        if(is_image){
            tile = createNewImageTile(x, y, low_quality);
        }else{
            tile = createNewVideoTile(x, y, low_quality);
        }
    }else{
        auto tile_id = MediaTile::tileId(sequence_, x, y, low_quality);
        auto it = tiles_.find(tile_id);
        if (it != tiles_.end()){
            tile = it->second;
        }else{
            if(is_image){
                tile = createNewImageTile(x, y, low_quality);
            }else{
                tile = createNewVideoTile(x, y, low_quality);
            }
            tiles_[tile_id] = tile;
        }
    }
    return tile;
}


std::shared_ptr<MediaTile>
CapturedFrame::createNewVideoTile(int x, int y, bool low_quality){
    int matrix_x = ParametersProducer::matrixNumX();
    int matrix_y = ParametersProducer::matrixNumY();
    
    int mw = width_ / matrix_x;
    int mh = height_ / matrix_y;
    int lx = x * mw;
    int ly = y * mh;
    
    std::vector<cv::Mat> frames;
    int width = mw; int height = mh;
    for(auto it : frame_vector_){
        cv::Mat roi_img(it, cv::Rect(lx, ly, mw, mh));
        if(low_quality){
            cv::resize(roi_img, roi_img, cv::Size(), 0.707, 0.707);
            width = roi_img.cols; height = roi_img.rows;
        }
        frames.push_back(roi_img);
    }

    auto tile = std::make_shared<VideoTile>(MediaTile::tileId(sequence_, x, y, low_quality), std::move(frames), width, height);
    return tile;
}



std::shared_ptr<MediaTile>
CapturedFrame::createNewImageTile(int x, int y, bool low_quality){
    int matrix_x = ParametersProducer::matrixNumX();
    int matrix_y = ParametersProducer::matrixNumY();
    
    int mw = width_ / matrix_x;
    int mh = height_ / matrix_y;
    int lx = x * mw;
    int ly = y * mh;
    
    std::vector<cv::Mat> frames;
    int width = mw; int height = mh;
    auto it = frame_vector_.back();
    cv::Mat roi_img(it, cv::Rect(lx, ly, mw, mh));
    if(low_quality){
        cv::resize(roi_img, roi_img, cv::Size(), 0.707, 0.707);
        width = roi_img.cols; height = roi_img.rows;
    }
    frames.push_back(roi_img);

    auto tile = std::make_shared<ImageTile>(MediaTile::tileId(sequence_, x, y, low_quality), std::move(frames), width, height);
    return tile;
}
