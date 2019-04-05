/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#include <iterator>
#include <iostream>
#include <ndn-cxx/encoding/tlv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "ParametersProducer.h"
#include "ImageTile.h"

namespace fs = boost::filesystem;
namespace ipc = boost::interprocess;

const size_t MAX_SEGMENT_SIZE = ndn::MAX_NDN_PACKET_SIZE >> 1;


ImageTile::ImageTile(const std::string& tile_id, std::vector<cv::Mat>&& frames, int mw, int mh)
    : MediaTile(tile_id, std::move(frames), mw, mh),
      file_size_(0),
      encode_(false)
{
//    const auto filename =  tile_id + ParametersProducer::mediaExtension();
//    filepath_ =  fs::path(ParametersProducer::workingDir()/filename);
}

ImageTile::~ImageTile()
{
/*    boost::system::error_code error;
    if(fs::exists(filepath_, error)){
        fs::remove(filepath_);
    }
*/
}

void
ImageTile::encode()
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (encode_) return;

    auto frame = frames_.back();
    std::vector<int> param = { CV_IMWRITE_JPEG_QUALITY, ParametersProducer::quality() };
    image_ = std::make_unique<std::vector<uint8_t>>();
    cv::imencode(".jpg", frame, *image_, param);
    encode_ = true;
}

int
ImageTile::fileSize()
{
    encode();
    return (file_size_ > 0) ? file_size_ : (file_size_ = image_->size());
}

std::shared_ptr<std::vector<uint8_t>>
ImageTile::getSegment(uint32_t segment_no)
{
    auto filesize = fileSize();
    size_t offset = MAX_SEGMENT_SIZE * segment_no;
    if (offset > filesize) return nullptr;
   
    auto buffer = std::make_shared<std::vector<uint8_t>>(0);
    buffer->reserve(MAX_SEGMENT_SIZE);
    
    auto it = image_->begin() + offset;
    auto end_it = it + MAX_SEGMENT_SIZE;
    if (end_it > image_->end()) end_it = image_->end();
    std::copy(it, end_it, std::back_inserter(*buffer));

    return buffer;
}

