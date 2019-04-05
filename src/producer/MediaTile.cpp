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
#include "MediaTile.h"

const size_t MAX_SEGMENT_SIZE = ndn::MAX_NDN_PACKET_SIZE >> 1;


std::string
MediaTile::tileId(uint32_t seq, int x, int y, bool low_quality)
{
    int block_id =  y * ParametersProducer::matrixNumX() + x;
    return low_quality ? std::to_string(seq) + "_" + std::to_string(block_id) + "L"
                        : std::to_string(seq) + "_" + std::to_string(block_id);
}


MediaTile::MediaTile(const std::string& tile_id, std::vector<cv::Mat>&& frames, int mw, int mh)
    : frames_(std::move(frames)),
      width_(mw),
      height_(mh)
{
}


MediaTile::~MediaTile()
{
}


uint32_t
MediaTile::finalSegmentNo() {
    return (fileSize() / MAX_SEGMENT_SIZE);
};
