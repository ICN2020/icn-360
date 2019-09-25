/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once

#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <ndn-cxx/face.hpp>
#include "Camera.h"
#include "Signature.h"
#include "ConsumerCounter.h"

class Producer : ndn::noncopyable
{
private:
  ndn::Face face_;
  Camera& camera_;
  Signature signature_;
  std::vector<std::thread> threads_;
  boost::asio::io_service interest_queue_;
  std::shared_ptr<boost::asio::io_service::work> worker_;
  bool running_;
  std::string app_prefix_;
  ConsumerCounter consumer_counter_;

  ndn::Name meta_data_name_;
  ndn::Name img_data_name_;
  ndn::Name status_data_name_;
  ndn::Name::Component low_quality_component_;

public:
  Producer(Camera& camera, std::string prefix);
  ~Producer();

  void start();
  bool isRunning(){ return running_; };

private:
  void onInterest(const ndn::InterestFilter& filter, const ndn::Interest& interest);
  
  void onRegisterFailed(const ndn::Name& prefix, const std::string& reason);
  void sendMetaData(const ndn::Name& name);
  void sendImageData(const ndn::Interest& name);
  void sendStatusData(const ndn::Name& name);
  
  void processInterest(const ndn::Interest& name);
};
