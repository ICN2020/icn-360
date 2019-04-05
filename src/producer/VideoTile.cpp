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
#include <opencv2/videoio.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "ParametersProducer.h"
#include "VideoTile.h"

namespace fs = boost::filesystem;
namespace ipc = boost::interprocess;

const size_t MAX_SEGMENT_SIZE = ndn::MAX_NDN_PACKET_SIZE >> 1;


VideoTile::VideoTile(const std::string& tile_id, std::vector<cv::Mat>&& frames, int mw, int mh)
    : MediaTile(tile_id, std::move(frames), mw, mh),
      file_size_(0),
      encode_(false)
{
    const auto filename =  tile_id + ParametersProducer::mediaExtension();
    filepath_ =  fs::path(ParametersProducer::workingDir()/filename);
}

VideoTile::~VideoTile()
{
    boost::system::error_code error;
    if(fs::exists(filepath_, error)){
        fs::remove(filepath_);
    }
}

void
VideoTile::encode()
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (encode_) return;

    double fps = static_cast<double>(frames_.size()) / (static_cast<double>(ParametersProducer::videoSegmentSize()) / 1000.0f);
    auto writer = cv::VideoWriter(filepath_.string(), ParametersProducer::videoCodec(), fps, cv::Size(width_, height_));
    for(auto f : frames_){
        writer << f;
    }
    writer.release();
    encode_ = true;
}

int
VideoTile::fileSize()
{
    encode();
    return (file_size_ > 0) ? file_size_ : (file_size_ = fs::file_size(filepath_));
}

std::shared_ptr<std::vector<uint8_t>>
VideoTile::getSegment(uint32_t segment_no)
{
    auto filesize = fileSize();
    size_t offset = MAX_SEGMENT_SIZE * segment_no;
    if (offset > filesize) return nullptr;
    size_t size = (offset + MAX_SEGMENT_SIZE) > filesize ? (filesize - offset) : MAX_SEGMENT_SIZE;
    
    ipc::file_mapping map(filepath_.c_str(), ipc::read_only);
    ipc::mapped_region view(map,ipc::read_only, offset, size);

    auto buffer = std::make_shared<std::vector<uint8_t>>(view.get_size());
    memcpy(buffer->data(), view.get_address(), view.get_size());

    return buffer;
}

