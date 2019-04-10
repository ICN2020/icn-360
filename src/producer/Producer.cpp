/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <string>
#include <ndn-cxx/version.hpp>
#include <ndn-cxx/util/time.hpp>
#include <cereal/archives/json.hpp>
#include <cryptopp/randpool.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/seckey.h>
#include <cryptopp/secblock.h>
#include <cryptopp/modes.h>
#include "ParametersProducer.h"
#include "CapturedFrame.h"
#include "Constants.h"
#include "CameraMetadata.h"
#include "ProducerStatusData.h"
#include "MediaTile.h"
#include "Producer.h"

/*
const auto APP_PREFIX_NAME = ndn::Name(APP_PREFIX);
const auto META_DATA_NAME = ndn::Name(META_DATA_PATH);
const auto IMG_DATA_NAME = ndn::Name(IMG_DATA_PATH);
*/

Producer::Producer(Camera& camera, std::string prefix)
  : camera_(camera),
    signature_(prefix.c_str()),
    threads_(ParametersProducer::threadCount()),
    interest_queue_(),
    worker_(nullptr),
    running_(true),
    app_prefix_(prefix),
    consumer_counter_()
{
  meta_data_name_ = ndn::Name(app_prefix_).append(META_DATA_SUFFIX);
  img_data_name_ = ndn::Name(app_prefix_).append(IMG_DATA_SUFFIX);
  status_data_name_ = ndn::Name(app_prefix_).append(STATUS_DATA_SUFFIX);
  low_quality_component_ = ndn::Name::Component(LOW_DATA_PREFIX);

  //std::cout << "META_NAME: " << meta_data_name_.toUri() << ", IMG_NAME: " << img_data_name_.toUri() << std::endl;
}



Producer::~Producer()
{

}

void
Producer::start()
{
  running_ = true;
  face_.setInterestFilter(app_prefix_,
                              std::bind(&Producer::onInterest, this, _1, _2),
                              ndn::RegisterPrefixSuccessCallback(),
                              std::bind(&Producer::onRegisterFailed, this, _1, _2));
                             
  if(worker_ != nullptr){
    worker_.reset();
  }
  worker_ = std::make_shared<boost::asio::io_service::work>(interest_queue_);

  for (int i = 0; i < ParametersProducer::threadCount(); i++) {
    threads_[i] = std::thread( [this]{ this->interest_queue_.run(); });
  }
  face_.processEvents();
}


void
Producer::onInterest(const ndn::InterestFilter& filter, const ndn::Interest& interest)
{
  
  const ndn::Name& name = interest.getName();
  interest_queue_.post([this, name]{ this->processInterest(name); });
}


void
Producer::processInterest(const ndn::Name& name)
{
  //std::cout << name.toUri() << std::endl;

  if(meta_data_name_.isPrefixOf(name) == true){
    consumer_counter_.increase();
    sendMetaData(name);
  }else if(img_data_name_.isPrefixOf(name) == true){
    sendImageData(name);
  }else if(status_data_name_.isPrefixOf(name) == true){
    sendStatusData(name);
  }else{
    return;
  }
}

void
Producer::sendMetaData(const ndn::Name& name)
{
  if (name[NAME_SEGMENT_LOCATION].isSegment() && name[NAME_SEGMENT_LOCATION].toSegment() > 0) return;
  CameraMetadata metadata;
  camera_.getCamereaMetadata(metadata);
  metadata.mui = METADATA_UPDATE_INTERVAL;

  std::stringstream ss;
  {
    cereal::JSONOutputArchive o_archive(ss);
    o_archive(cereal::make_nvp("root", metadata));
  }
  
  std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>();
  data->setName(name);
  data->setFreshnessPeriod(ndn::time::milliseconds(0));
#if NDN_CXX_VERSION > 6002
  data->setFinalBlock(ndn::name::Component::fromSegment(0));
#else
  data->setFinalBlockId(ndn::name::Component::fromSegment(0));
#endif
  const std::string content = ss.str();
  data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());

  signature_.sign(*data);
  face_.put(*data);
}


void
Producer::sendStatusData(const ndn::Name& name)
{
  if (name[NAME_SEGMENT_LOCATION].isSegment() && name[NAME_SEGMENT_LOCATION].toSegment() > 0) return;
  
  ProducerStatusData status;
  status.consumers = consumer_counter_.get();

  std::stringstream ss;
  {
    cereal::JSONOutputArchive o_archive(ss);
    o_archive(cereal::make_nvp("root", status));
  }
  
  std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>();
  data->setName(name);
  data->setFreshnessPeriod(ndn::time::milliseconds(0));
#if NDN_CXX_VERSION > 6002
  data->setFinalBlock(ndn::name::Component::fromSegment(0));
#else
  data->setFinalBlockId(ndn::name::Component::fromSegment(0));
#endif
  const std::string content = ss.str();
  data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());

  signature_.sign(*data);
  face_.put(*data);
}


void
Producer::sendImageData(const ndn::Name& name)
{
  if (!name[NAME_COORDINATE_Y_LOCATION].isNumber()) return;
  uint32_t y = name[NAME_COORDINATE_Y_LOCATION].toNumber();
  if (!name[NAME_COORDINATE_X_LOCATION].isNumber()) return;
  uint32_t x = name[NAME_COORDINATE_X_LOCATION].toNumber();
  if (!name[NAME_SEQUENCE_LOCATION].isNumber()) return;
  uint32_t sequence_no = name[NAME_SEQUENCE_LOCATION].toNumber();
  if (!name[NAME_SEGMENT_LOCATION].isSegment()) return;
  auto segment_no = name[NAME_SEGMENT_LOCATION].toSegment();
  bool low_quality = name[NAME_QUALITY_LOCATION].equals(low_quality_component_);
  //std::cout << name.to << std::endl;

  CameraMetadata metadata;
  camera_.getCamereaMetadata(metadata);

  auto frame = camera_.getCapturedFrame(sequence_no);
  if(frame == nullptr){
    ndn::Interest interest(name);
    auto nack = ndn::lp::Nack(interest);
    nack.setReason(ndn::lp::NackReason::CONGESTION);
    face_.put(nack);
    return;
  }
  
  auto image = frame->getMediaTile(x, y, low_quality);
  if(frame == nullptr){
    ndn::Interest interest(name);
    auto nack = ndn::lp::Nack(interest);
    nack.setReason(ndn::lp::NackReason::DUPLICATE);
    face_.put(nack);
    return;
  }
  
  auto segment = image->getSegment(segment_no);
  if(segment == nullptr){
    ndn::Interest interest(name);
    auto nack = ndn::lp::Nack(interest);
    nack.setReason(ndn::lp::NackReason::NO_ROUTE);
    face_.put(nack);
    return;
  }
  
  auto data = std::make_shared<ndn::Data>();
  data->setName(name);
  data->setFreshnessPeriod(ndn::time::milliseconds(metadata.fiv * 10));
#if NDN_CXX_VERSION > 6002
  data->setFinalBlock(ndn::name::Component::fromSegment(image->finalSegmentNo()));  
#else
  data->setFinalBlockId(ndn::name::Component::fromSegment(image->finalSegmentNo()));  
#endif
  data->setContent(segment->data(), segment->size());

  signature_.sign(*data);
  face_.put(*data);
}



void
Producer::onRegisterFailed(const ndn::Name& prefix, const std::string& reason)
{
  std::cerr << "ERROR: Failed to register prefix \""
            << prefix << "\" in local hub's daemon (" << reason << ")"
            << std::endl;
  face_.shutdown();
}
