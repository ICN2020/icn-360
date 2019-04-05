/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

FrameDownloader = function(face, metadata, rttStatistics){
    if(!(this instanceof FrameDownloader)){ return new FrameDownloader(hostname, port, field_of_view); }

    this.face = face;
    this.metadata = metadata;
    this.download_start_time = null;
    this.download_queue = [];
    this.remaining_tiles_num = 0;
    this.media_array = new Array();
    this.rttStatistics = rttStatistics;
    this.cb = undefined;
}


FrameDownloader.prototype.downloadFrame = function(sequence_number, fov, callback){
    this.download_start_time = new Date();
    this.sequence_number = sequence_number;
    this.cb = callback;

    let tile_num = this.metadata.width * this.metadata.height + 1;
    this.download_queue = [].concat(fov.sort(function(a,b){
        return (((a.edge  === undefined) ? tile_num : 0) + a.x + a.y) < (((b.edge  === undefined) ? tile_num : 0) + b.x + b.y)}));
    this.remaining_tiles_num = this.download_queue.length;
    
    for(let i=0; i<Parameters.DOWNLOAD_PIPELINES; i++){
        this.downloadNextTiledata();
    }
}

FrameDownloader.prototype.downloadNextTiledata = function(){
    if(this.download_queue.length > 0){
        let it = this.download_queue.shift();
        if(Parameters.HTTP_MODE == false){
            this.downloadByNDN(it);
        }else{
        //    this.downloadByHTTP(it); // 
        }
    }
}

FrameDownloader.prototype.downloadByNDN = function(tile){
    let name = new Name(Parameters.IMG_DATA_PREFIX);
    if(tile.edge !== undefined && tile.edge == true && Parameters.EDGE_MARK == true){
        name.append("f:transcoding");
    }
    name.append(Name.Component.fromNumber(this.sequence_number));
    name.append(Name.Component.fromNumber(tile.x));
    name.append(Name.Component.fromNumber(tile.y));
        
    let downloader = new FixedRateDownloader(this.face, name, this.rttStatistics, this.metadata.mime, this.onGetTiledata.bind(this, tile.x, tile.y));
    downloader.startDownload();
}

FrameDownloader.prototype.onGetTiledata = function(x, y, imageData){
    let mx = (this.metadata.width * x) / this.metadata.tile.value0;
    let my = (this.metadata.height * y) / this.metadata.tile.value1;
    let wx = this.metadata.width / this.metadata.tile.value0;
    let wy = this.metadata.height / this.metadata.tile.value1;
    
    this.remaining_tiles_num--;
    if(imageData !== undefined){
        let loadMediaData;
        if(this.metadata.mime.startsWith('video/')){
            loadMediaData = (url) => {
                return new Promise((resolve, reject) => {
                    let video = document.createElement("video");
                    video.preload = "none";
                    video.muted = true;
                    video.loop = false;
                    video.oncanplaythrough = () => {
                        video.play();
                        video.pause();
                        video.onerror = null;
                        resolve(video);
                    }
                    video.onerror = (e) => reject(e);
                    video.src = url;
                })
            }
        }else{
            loadMediaData = (url) => {
                return new Promise((resolve, reject) => {
                    let image = new Image;
                    image.onload = () => resolve(image);
                    image.onerror = (e) => reject(e);
                    image.src = url;
                })
            }
        }

        loadMediaData(imageData).then(res => {
            this.appendMedia(this.sequence_number, x, y, res);

            if(this.remaining_tiles_num <= 0){
                this.cb(this);
            }
        }).catch( e => {
            console.log(e);
            URL.revokeObjectURL(imageData); 
            if(this.remaining_tiles_num <= 0){
                this.cb(this);
            }
        })
    }
    
    if(this.remaining_tiles_num > 0){
        setTimeout(this.downloadNextTiledata(), 0);
    }
}

FrameDownloader.prototype.getMediaArray = function() {
    return this.media_array;
}

FrameDownloader.prototype.appendMedia = function(sequence_number, x, y, data) {
    let key_name = String(x) + "," + String(y);
    this.media_array[key_name] = data; 
}

FrameDownloader.prototype.revoke = function(sequence_number, x, y, data) {
    for(var key_name in this.media_array){
        let src = this.media_array[key_name].src;
        if(src.startsWith("blob:")){
            URL.revokeObjectURL(src);
        }        
    }; 
}
