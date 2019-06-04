#!/bin/bash
echo "Download required libraries for ICN-360-JS-SERVER."
# ndn-js
curl -O https://raw.githubusercontent.com/named-data/ndn-js/master/build/ndn.min.js
# three-js
curl -O https://raw.githubusercontent.com/mrdoob/three.js/dev/build/three.min.js
curl -O https://raw.githubusercontent.com/mrdoob/three.js/dev/examples/js/controls/OrbitControls.js
curl -O https://raw.githubusercontent.com/mrdoob/three.js/dev/examples/js/controls/DeviceOrientationControls.js
curl -O https://raw.githubusercontent.com/mrdoob/three.js/dev/examples/js/effects/StereoEffect.js
# mdl
curl -O https://code.getmdl.io/1.3.0/mdl.zip
unzip mdl.zip
# dialog
curl -O https://raw.githubusercontent.com/GoogleChrome/dialog-polyfill/master/dist/dialog-polyfill.js
curl -O https://raw.githubusercontent.com/GoogleChrome/dialog-polyfill/master/dialog-polyfill.css

# box-icons
curl -O https://raw.githubusercontent.com/google/material-design-icons/master/iconfont/material-icons.css
curl -O https://raw.githubusercontent.com/google/material-design-icons/master/iconfont/MaterialIcons-Regular.eot
curl -O https://raw.githubusercontent.com/google/material-design-icons/master/iconfont/MaterialIcons-Regular.woff2
curl -O https://raw.githubusercontent.com/google/material-design-icons/master/iconfont/MaterialIcons-Regular.woff
curl -O https://raw.githubusercontent.com/google/material-design-icons/master/iconfont/MaterialIcons-Regular.ttf
