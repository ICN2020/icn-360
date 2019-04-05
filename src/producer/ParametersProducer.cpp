/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#include <iostream>
#include <thread>
#include <map>
#include <utility>
#include <boost/program_options.hpp>
#include <opencv2/videoio.hpp>
#include "ParametersProducer.h"

namespace fs = boost::filesystem;

ParametersProducer::ParametersProducer()
    :   camera_width_(0),
        camera_height_(0),
        matrix_num_x_(10),
        matrix_num_y_(10),
        quality_(70),
        thread_count_(std::thread::hardware_concurrency()),
        debug_filename_(""),
        signature_type_(SignatureType::SHA_256),
        app_prefix_(""),
        video_segment_size_(1000),
        working_dir_(""),
        is_loop_(false),
        media_codec_(MediaCodicType::JPEG)
{

}

bool
ParametersProducer::parseProgramOptions(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description desc("360 degree video streaming producer on NDN");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>()->default_value("0"), "camera's device id or video filename")
        ("matrix,m", po::value<int>()->default_value(10), "matrix size")
        ("prefix-name,n", po::value<std::string>()->default_value("/icn2020.org/theta"), "name prefix of produced video")
        ("resolution,r", po::value<std::string>()->default_value("NONE"), "camera resolution")
        ("signature-type,s",  po::value<std::string>()->default_value("SHA_256"), "signature type")
        ("threads,t",  po::value<int>()->default_value(std::thread::hardware_concurrency() - 1), "the number of threads")
        ("video-segment-size,f",  po::value<int>()->default_value(500), "time length of one content object [ms]")
        ("playback-loop,l",  "playback loop option for video file")
        ("work-dir,w", po::value<std::string>()->default_value("/tmp/i360"), "working directory")
        ("codec,c", po::value<std::string>()->default_value("jpeg"), "video codec");
    
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch (const po::error_with_option_name& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    po::notify(vm);
    
    if (vm.count("help")) {
        std::cout << desc <<std::endl;
        return false;
    }
    try {
        input_device_ = vm["input"].as<std::string>();
        matrix_num_x_ = matrix_num_y_ = vm["matrix"].as<int>();
        app_prefix_ = vm["prefix-name"].as<std::string>();
        setCameraResolutionFromString(vm["resolution"].as<std::string>());
        signature_type_ = SignatureType::signatureFromKey(vm["signature-type"].as<std::string>());
        thread_count_ = vm["threads"].as<int>();
        video_segment_size_ = vm["video-segment-size"].as<int>();
        is_loop_ = vm.count("playback-loop") ? true : false;
        setVideoCodecFromString(vm["codec"].as<std::string>());
        if(!makeWorkingDir(vm["work-dir"].as<std::string>())){
            return false;
        }
    } catch(const boost::bad_any_cast& e) {
		std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}


bool
ParametersProducer::makeWorkingDir(std::string dirname)
{
    boost::system::error_code error;
    
    working_dir_ = boost::filesystem::path(dirname);
    if(! fs::exists(working_dir_)) {
        if(!fs::create_directories(working_dir_, error)){
            std::cerr << "Cannot create directory: " << working_dir_ << std::endl;
            return false;
        }
    }
    if(!fs::is_directory(working_dir_)){
        std::cerr << working_dir_ << "is not a directory" << std::endl;
        return false;
    }
    return true;
}

void
ParametersProducer::setCameraResolutionFromString(std::string s){
    typedef std::pair<int, int> Resolution;
    const std::map<std::string, Resolution> resolutionName = {
        {"NONE", Resolution(0, 0)},
        {"4k", Resolution(3840, 1920)},
        {"4K", Resolution(3840, 1920)},
        {"RICOH-R", Resolution(1920, 960)},
        {"THETA-V", Resolution(1920, 960)},
        {"THETA-S-L", Resolution(1920, 960)},
        {"THETA-S-M", Resolution(1280, 720)}
    };
    auto it = resolutionName.find(s);
    if(it != resolutionName.end()){
        camera_width_ = it->second.first;
        camera_height_ = it->second.second;   
    }else{
        std::cerr << "Unknown resolution : " << s << std::endl;
    }
}

SignatureType
SignatureType::signatureFromKey(std::string key)
{
    const std::map<std::string, Type> signatureName = {
        {"0", SHA_256},
 //       {"1", RSA_1024},
        {"2", RSA_2048},
        {"SHA_256", SHA_256},
//        {"RSA_1024", RSA_1024},
        {"RSA_2048", RSA_2048},
        {"ECDSA_224", ECDSA_224},
        {"ECDSA_256", ECDSA_256},
        {"ECDSA_384", ECDSA_384},
        {"HMAC", HMAC_SHA_256}
    };
    auto it = signatureName.find(key);
    if(it != signatureName.end()){
        return it->second;
    }else{
        std::cerr << "Cannot find signature : " << key << std::endl;
        return NO_SIGN;
    }
}

int
SignatureType::keySize()
{
    int ks;
    switch(type_){
    case RSA_2048:
        ks = 2048;
        break;
    case ECDSA_224:
        ks = 224;
        break;
    case ECDSA_256:
        ks = 256;
        break;
    case ECDSA_384:
        ks = 384;
        break;
    default:
        ks = 0;
        break;
    }
    return ks;
}


void
ParametersProducer::setVideoCodecFromString(std::string s){
    media_codec_ = MediaCodicType::mediaCodicFromName(s);
}


MediaCodicType::MediaCodicType(MediaType t)
    : type_(t)
{
    switch(type_){
        case JPEG:
            extention_ = ".jpg";
            fourcc_ = 0;
            break;
        case MP4:
            extention_ = ".mp4";
            fourcc_ = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
            break;
        case H264:
            extention_ = ".mp4";
            fourcc_ = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
            break;
        default:
            extention_ = ".bim";
            fourcc_ = 0;
            break;
    }
}


MediaCodicType
MediaCodicType::mediaCodicFromName(std::string name) 
{
    const std::map<std::string, MediaCodicType> mediaCodic = {
        {"JPEG", JPEG},
        {"jpeg", JPEG},
        {"JPG", JPEG},
        {"jpg", JPEG},
        {"MP4", MP4},
        {"mp4", MP4},
        {"h264", H264},
        {"H264", H264},
    };
    auto it = mediaCodic.find(name);
    if(it != mediaCodic.end()){
        return it->second;
    }else{
        std::cerr << "Cannot find codic : " << name << std::endl;
        return JPEG;
    }
}