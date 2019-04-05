consumer js
===
360 degree video streaming consumer on NDN.js. Currently only Equirectangular is supported.

### Required Library

You should downalod required libraries and put in `libs` folder

- ndn.js   : NDN Liburary
- THREE.js : 3D Liburary based on WebGL
- Material Dedign Lite : GUI Tools
- dialog-polyfill : UI
- Material Design Icons : Icons 

please read [`libs/README.md`](/src/consumer-js/libs/README.md)

### How to use
#### consumer

Please put `consumer-js` folder in your web site where nfd with WebSocket runs, and access it by a web browser.
One simple way is use http server module of Python.

```
> cd src/consumer-js
> python3 -m http.server 8000
```
