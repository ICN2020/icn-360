/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * @license MIT License
 * @copyright 2019 KDDI Research, Inc.
 */
#pragma once
#include <memory>


class CapturedFrame;
struct CameraMetadata;

class Camera
{
public:
    Camera(){};
    virtual ~Camera(){};
  
    virtual std::shared_ptr<CapturedFrame> getCapturedFrame(uint32_t sequenceNo) = 0;
    virtual void getCamereaMetadata(CameraMetadata& metadata) = 0;
};
