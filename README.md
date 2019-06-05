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

### References
1. A. Tagami, K. Ueda, R. Lukita, J. D. Benedetto, M. Arumaithurai, G. Rossi, A. Detti and T. Hasegawa, "Tile-based Panoramic Live Video Streaming on ICN," In Proceedings of IEEE International Conference on Communications Workshops, ICC Workshops RAFNET, Shanghai, China, May 2019.
1. A. Tagami, "[Demo: Panoramic Video Streaming via Edge-Computing and ICN](https://datatracker.ietf.org/meeting/interim-2018-icnrg-02/session/icnrg)," ICNRG Interim Meeting , Montreal, Canada, Jul. 2018.
1. "[e360: Panoramic Video Streaming via Edge-Computing and Future Internet](http://www.icn2020.org/e360/)," CEBIT 2018, Hannover, Germany, Jun. 2018.
1. A. Tagami, K. Ueda, R. Lukita, J. D. Benedetto, M. Arumaithurai, G. Rossi and A. Detti, "[Demo: Edge Transcoding with Name-based Routing](https://dl.acm.org/citation.cfm?doid=3267955.3269008)," In Proceedings of ACM Conference on Information-Centric Networking, ACM ICN, pp. 218 - 219, Boston, MA, Sep. 2018.
1. K. Ueda, Y. Ishugaki, A. Tagami and T. Hasegawa, "[Demo: Panoramic Streaming using Named Tiles](https://dl.acm.org/citation.cfm?doid=3125719.3132093)," In Proceedings of ACM Conference on Information-Centric Networking, ACM ICN, pp. 204 - 205, Berlin, Germany, Sep. 2017.

