/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

PanoramaView = function(container_id, field_of_view) {
    if(!(this instanceof PanoramaView)){ return new PanoramaView(container_id, canvas_id, field_of_view); }

    this.enableVR = ('xr' in navigator && 'supportsSession' in navigator.xr) || ('getVRDisplays' in navigator);

    this.camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 1, 1100);
    this.camera.target = new THREE.Vector3(0,0,0);
    this.camera.position.set(0,0,0.1);
    
    this.scene = new THREE.Scene();
    this.render = new THREE.WebGLRenderer();
    this.render.setPixelRatio(window.devicePixelRatio);
	this.render.setSize(window.innerWidth, window.innerHeight);
    
    this.effect = new THREE.StereoEffect(this.render);
    let container = document.getElementById(container_id);
    container.appendChild(this.render.domElement);
    if(this.enableVR){
        this.render.vr.enabled = true;
        container.appendChild(THREE.WEBVR.createButton(this.render, {referenceSpaceType:'local'}));
    }
    this.surface = null;
    this.initScene();
    if(!this.enableVR){
        this.initControl();
    }
    this.start_time = new Date();

    this.field_of_view = field_of_view;
    this.consumer = undefined;
}

PanoramaView.prototype.initScene = function(){
    let geometry = new THREE.SphereBufferGeometry( 500, 60, 40 );
	geometry.scale( - 1, 1, 1 );

    let canvas = document.createElement('canvas');
    canvas.width = '640';
    canvas.height = '480';
    this.surface = canvas;
    this.texture = new THREE.Texture(canvas);
    this.texture.needsUpdate = true;
    this.texture.minFilter = THREE.LinearFilter;
    let material = new THREE.MeshBasicMaterial( {
        map: this.texture
	} );
    this.mesh = new THREE.Mesh( geometry, material );
    this.scene.add(this.mesh);
    window.addEventListener('resize', this.onWindowResize.bind(this), false);
}

PanoramaView.prototype.initControl = function(){

    let ua = navigator.userAgent;

    let prevCamera = this.camera;

    this.camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 1, 1100);
    this.camera.target = new THREE.Vector3(0,0,0);
    this.camera.position.copy( prevCamera.position );
    this.camera.rotation.copy( prevCamera.rotation );

    if(Parameters.DEVICE_ORIENTATION && ((ua.indexOf('iPhone') > 0 || ua.indexOf('iPod') > 0 || ua.indexOf('iPad') > 0 || ua.indexOf('Android') > 0)) ) {
        this.controls = new THREE.DeviceOrientationControls(this.camera, this.render.domElement);
    } else {
        this.controls = new THREE.OrbitControls(this.camera, this.render.domElement);
    }
    
}


PanoramaView.prototype.setConsumer = function(consumer){
    this.consumer = consumer;
}


PanoramaView.prototype.animate = function(){
    if(this.enableVR){
        this.render.setAnimationLoop(this.animate.bind(this));
    }else{
        window.requestAnimationFrame(this.animate.bind(this));
    }
    this.update();
    this.field_of_view.setAnimationInterval((new Date()) - this.start_time);
    this.start_time = new Date();
}


PanoramaView.prototype.update = function(){
    if(this.consumer !== undefined && this.consumer.isUpdate()){
        this.mesh.material.map.needsUpdate = true;
    }
    if(Parameters.VR_MODE){
           this.effect.render(this.scene, this.camera);
       }else{
          this.render.render(this.scene, this.camera);
    }
    if(!this.enableVR){
       this.controls.update();
    }
    this.field_of_view.calcFieldOfView(500, this.camera);
}


PanoramaView.prototype.onWindowResize = function(){
    this.camera.aspect = window.innerWidth / window.innerHeight;
    this.camera.updateProjectionMatrix();

    this.render.setSize( window.innerWidth, window.innerHeight );
    this.effect.setSize( window.innerWidth, window.innerHeight );
}


PanoramaView.prototype.getSurface = function(){
    console.log(this.surface);
    return this.surface;
}