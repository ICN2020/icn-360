/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

 FieldOfView = function(){
    if(!(this instanceof FieldOfView)){ return new FieldOfView(); }

    this.mx = undefined;
    this.my = undefined;

    this.animationInterval = 0;
    this.fov = [];
    this.geometry = [];
}

FieldOfView.prototype.setTileSize = function(mx, my){
    this.mx = mx;
    this.my = my;
}

FieldOfView.prototype.setAnimationInterval = function(p){
    this.animationInterval = p;
}

FieldOfView.prototype.animationInterval = function(p){
    return(this.animationInterval);
}


FieldOfView.prototype.createGeometry = function(radius){
    if (typeof this.mx == 'undefined' || typeof this.my == 'undefined') {
        return false;
    }

    let pi_2 = Math.PI * 2;

    for(let iy = 0; iy <= this.my; iy++){
        let v = iy / this.my;
        for(let ix = 0; ix <= this.mx; ix++){
            let u = ix / this.mx;
            
            let x = radius * Math.cos(u * pi_2) * Math.sin(v * Math.PI);
			let y = radius * Math.cos(v * Math.PI);
            let z = radius * Math.sin(u * pi_2) * Math.sin(v * Math.PI);
            this.geometry.push(new THREE.Vector3(x, y, z));
        }
    }
}

FieldOfView.prototype.calcFieldOfView = function(radius, camera){
    function gridToPos(x, y, mx){
        return x + (y * (mx + 1));
    }

    if (this.geometry.length == 0){
        if(!this.createGeometry(radius)){
            return false;
        }
    }

    camera.updateMatrix(); 
    camera.updateMatrixWorld();
                                                                                                                     
    let frustum = new THREE.Frustum();
    frustum.setFromMatrix(new THREE.Matrix4().multiplyMatrices(camera.projectionMatrix, camera.matrixWorldInverse));
    let flag = [];
    for(let i = 0; i < this.geometry.length; i ++){
        flag.push(frustum.containsPoint(this.geometry[i]) ? 1 : 0);
    }
    let fov = [];
    for(let iy = 0; iy < this.my; iy++){
        for(let ix = 0; ix < this.mx; ix++){
            let weight = flag[gridToPos(ix, iy, this.mx)] + flag[gridToPos(ix+1, iy, this.mx)]
                       + flag[gridToPos(ix, iy+1, this.mx)] + flag[gridToPos(ix+1, iy+1, this.mx)];
            if(weight > 2){
                fov.push({x: ix, y: iy});
            }else if(weight > 0){
                fov.push({x: ix, y: iy, edge: true});    
            }
        }
    }
    this.fov = fov;
}

