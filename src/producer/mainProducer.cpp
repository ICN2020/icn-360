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
#include <boost/lexical_cast.hpp>
#include "CvCamera.h"
#include "Producer.h"
#include "ParametersProducer.h"
#include "Constants.h"


int
main(int argc, char** argv)
{
  
  if(ParametersProducer::getInstance().parseProgramOptions(argc, argv) == false){
    return EXIT_FAILURE;
  }
  const std::string& input_device = ParametersProducer::inputDevice();
  std::unique_ptr<Camera> camera = nullptr;

  auto cv_camera = std::unique_ptr<CvCamera>(new CvCamera());
  try {
    int device_id = boost::lexical_cast<int>(input_device);
    if (!cv_camera->open(device_id, ParametersProducer::cameraWidth(), ParametersProducer::cameraHeight(), 0)){
      std::cerr << "Can't open capture device:" << device_id << std::endl;
      return EXIT_FAILURE;
    }
  }catch(const boost::bad_lexical_cast& e){
    if (!cv_camera->open(input_device)){
      std::cerr << "Can't open capture device:.\n";
      return EXIT_FAILURE;
    }
  }
  cv_camera->start();
  camera = std::move(cv_camera);


  std::string app_prefix = std::string(APP_PREFIX);
  if(ParametersProducer::appPrefix() != "")
    app_prefix = ParametersProducer::appPrefix();
  std::cout << app_prefix << std::endl;
  Producer producer(*camera, app_prefix);
  try {
    sleep(3);
    producer.start();
    /*while(producer.isRunning()){
      sleep(10);
    }
    producer.stop();*/
  }catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return EXIT_SUCCESS;
}