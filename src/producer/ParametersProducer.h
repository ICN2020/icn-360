/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#pragma once
#include <string>
#include <boost/filesystem.hpp>


// For CvCamera/DebugCamera
constexpr size_t FRAME_BUFFER_LENGTH = 10; // 5 * frameInterval
constexpr uint32_t DEFAULT_FRAME_INTERVAL_TIME = 70;
constexpr uint32_t FRAME_INTERVAL_REFRESH = 10000; // 10000 ms (10s)
constexpr uint32_t METADATA_UPDATE_INTERVAL = 1000; // 1000 ms (1s)

constexpr int RSA_SIGNATURE_SIZE = 1024;

struct SignatureType {
    enum Type : unsigned int {
        NO_SIGN  = 0,
        SHA      = 1 << 0,
        RSA      = 1 << 1,
        ECDSA    = 1 << 2,
        HMAC     = 1 << 3,
        KEY_1    = 0x80 << 0,
        KEY_2    = 0x80 << 1,
        KEY_3    = 0x80 << 2,
        SHA_256  = SHA | KEY_1,
        RSA_2048 = RSA | KEY_2,
        ECDSA_224 = ECDSA | KEY_1,
        ECDSA_256 = ECDSA | KEY_2,
        ECDSA_384 = ECDSA | KEY_3,
        HMAC_SHA_256 = HMAC | KEY_1
    } type_;
    SignatureType(unsigned int v) : type_(static_cast<Type>(v)) {};
    operator Type() {return type_;};
    bool hasType(SignatureType type) {return ((type_ & type.type_) == type.type_); };
    static SignatureType signatureFromKey(std::string key);
    int keySize();
};

struct MediaCodicType {
    enum MediaType {
        JPEG, MP4, H264
    } type_;
    int fourcc_;
    std::string extention_;

    MediaCodicType(MediaType v);
    static MediaCodicType mediaCodicFromName(std::string name);
    operator MediaType() {return type_; };
    bool isImage() {return type_ == JPEG; };
    const int fourcc() { return fourcc_; };
    const std::string extension() { return extention_; };
    const std::string mime(){
        switch(type_){
            case JPEG:
                return "image/jpeg";
                break;
            case MP4:
            case H264:
                return "video/mp4";
                break; 
        }
    }
};

// Singleton Design Pattern
class ParametersProducer
{
private:
    std::string input_device_;
    int camera_width_;
    int camera_height_;
    int matrix_num_x_;
    int matrix_num_y_;
    int quality_;
    int thread_count_;
    std::string debug_filename_;
    SignatureType signature_type_;
    std::string app_prefix_;
    int video_segment_size_;
    boost::filesystem::path working_dir_;
    bool is_loop_;
    MediaCodicType media_codec_;

public:
    static ParametersProducer& getInstance(){
        static ParametersProducer instance;
        return instance;
    }

    bool parseProgramOptions(int argc, char** argv);

    static const std::string& inputDevice() { return ParametersProducer::getInstance().input_device_; };
    static int cameraWidth() { return ParametersProducer::getInstance().camera_width_; };
    static int cameraHeight() { return ParametersProducer::getInstance().camera_height_; };
    static int matrixNumX() { return ParametersProducer::getInstance().matrix_num_x_; };
    static int matrixNumY() { return ParametersProducer::getInstance().matrix_num_y_; };
    static int quality() { return ParametersProducer::getInstance().quality_; };
    static int threadCount() { return ParametersProducer::getInstance().thread_count_; };
    static std::string& debugFilename() { return ParametersProducer::getInstance().debug_filename_; };
    static SignatureType signatureType() { return ParametersProducer::getInstance().signature_type_; };
    static std::string& appPrefix() { return ParametersProducer::getInstance().app_prefix_; };
    static int videoSegmentSize() { return ParametersProducer::getInstance().video_segment_size_; };
    static const boost::filesystem::path& workingDir() { return ParametersProducer::getInstance().working_dir_; };
    static bool isLoop() {  return ParametersProducer::getInstance().is_loop_; };

    static MediaCodicType mediaCodec() { return ParametersProducer::getInstance().media_codec_; };
    static int videoCodec() { return ParametersProducer::getInstance().media_codec_.fourcc(); };
    static const std::string mediaExtension() { return ParametersProducer::getInstance().media_codec_.extension(); };
    
private:
    ParametersProducer();
    ~ParametersProducer() = default;

    bool makeWorkingDir(std::string dirname);
    void setCameraResolutionFromString(std::string s); 
    void setVideoCodecFromString(std::string s);

};
