icn-360
===
360 degree video streaming producer/consumer on NDN.

### Required Library

Required libraries can be installed from default repository of each distribution

- [libopencv-dev](https://github.com/opencv/opencv) : OpenCV3
- [libcereal-dev](https://github.com/USCiLab/cereal) : Serialization (serialize METAFILE to JSON)
- [ndn-cxx](https://github.com/named-data) : NDN C++ library

Homebrew installs OpenCV4 as default. Please use `opencv@3` instead of `opencv`


### How to use
#### producer

```
producer [-i device_id] [-m matrix_size] [-n app_prefix] [-r resolution] [-s signature_mode] [-t thread_num] [-f frame_interval] [-l] [-w working_dir] [-c codec]
```

|Option|Description|Default|
|:--:|:--|--:|
|i|Device ID of video device. /dev/video<device_id> or video filename/URL | 0|
|m|The number of divisions of a video frame. (<matrix_size> * <matrix_size>)|10|
|n|The name prefix of produced video. | /icn2020.org/theta|
|r|Camera Resolution (4k: 3840x1920, THETA-S-L: 1920x1080) See ParametersProducer.cpp for more option |-|
|s|Signature type. (SHA_256, RSA_2048, ECDSA_224) See ParametersProducer.cpp for more option |SHA_256|
|t|The number of threads. The default number is the same as the number of CPU cores|-|
|f| Time length of on content object [ms]. |1000|
|l|Playback loop option for video file  |false|
|w|Working directory for video encoding |/tmp/i360|
|c|Video codec (JPEG, MPEG, H264) |JPEG|

#### consumer-js

Please read [`src/consumer-js/README.md`](/src/consumer-js/README.md)

### License

This software is released under the MIT License, see LICENSE.txt.

### Contributors

- Mayutan Arumaithurai
- Jacopo De Benedetto
- Andrea Detti
- Giulio Rossi
