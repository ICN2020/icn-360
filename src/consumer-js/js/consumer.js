/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
Consumer = function(hostname, port, field_of_view, surface){
    if(!(this instanceof Consumer)){ return new Consumer(hostname, port, field_of_view); }

    this.face = new Face({host:hostname, port:port});
    this.metadata = undefined;

    this.base_time = undefined;
    this.field_of_view = field_of_view;
    this.sequence_number = 0;

    this.surface = surface;
    this.clientID = Math.floor((Math.random() * 1000000) + 1);
    this.rttStatistics = new RttStatistics();
    
    this.frame_downloader = [];
    this.current_frame = undefined;

    this.renew_surface = false;
    this.frame_time = 0;

    this.priv_interval_time = undefined;

    this.surface_timer = undefined;

    this.animation_rate = 0;
    this.is_video_tile = false;

    Interest.setDefaultCanBePrefix(true);
}

Consumer.prototype.updateMetaData = function(){
    let name = new Name(Parameters.MATA_DATA_PREFIX);
    name.append(Math.random().toString(36).slice(-8));
    let request = new Interest(name);
    request.setMustBeFresh(true);
    new_base_time = new Date();

    this.face.expressInterest(request,
        (interest, data) => {
            this.rttStatistics.afterReceiveData(interest, data);
            let string = new TextDecoder('utf-8').decode(data.content);
            let first = (typeof this.metadata === 'undefined');    
            this.metadata = JSON.parse(string).root;
            if(first){
                this.surface.setAttribute('width', this.metadata.width);
                this.surface.setAttribute('height', this.metadata.height);
            };
            this.field_of_view.setTileSize(this.metadata.tile.value0, this.metadata.tile.value1);
            this.base_time = new_base_time;

            this.is_video_tile = this.metadata.mime.startsWith('video/');
            setTimeout(this.updateMetaData.bind(this), Parameters.METADATA_UPDATE_INTERVAL);
        },
        (interest, data) => {
            this.rttStatistics.afterTimeout(interest);
            setTimeout(this.updateMetaData.bind(this), Parameters.METADATA_UPDATE_INTERVAL);
        }
    );

    this.rttStatistics.afterSendInterest(request);
}

Consumer.prototype.downloadImagedata = function(){
    if(typeof this.metadata == 'undefined' || this.field_of_view.fov.length == 0){
        setTimeout(this.downloadImagedata.bind(this), Parameters.DOWNLOAD_WAIT_INTERVAL); 
    } else {
        let start_time = new Date();
        let diff = Math.max(start_time - this.base_time, 0);
        let next_sequence_number = Math.max(Math.floor(diff / this.metadata.fiv) + this.metadata.seq - 1, 0);
	    if(this.sequence_number == next_sequence_number){
            console.log("skip");
	        setTimeout(this.downloadImagedata.bind(this), Parameters.DOWNLOAD_WAIT_INTERVAL);
	        return;
        }
        var downloader = new FrameDownloader(this.face, this.metadata, this.rttStatistics);
        downloader.downloadFrame(next_sequence_number, this.field_of_view.fov, this.onGetFramedata.bind(this, next_sequence_number));
        this.sequence_number  = next_sequence_number;
        this.frame_downloader.push(downloader);
        setTimeout(this.downloadImagedata.bind(this), this.metadata.fiv);
    }
}



Consumer.prototype.onGetFramedata = function(sequence_number, downloader){
    if(this.surface_timer === undefined){
        this.updateSurface();

    }
}


Consumer.prototype.updateSurface = function(){
    if(this.frame_downloader.length > 0){
        let downloader = this.current_frame;
        this.current_frame = this.frame_downloader.shift();
        if(downloader !== undefined){
            downloader.revoke();
        }
        this.priv_interval_time = undefined;
        if(!this.is_video_tile){
            this.update();
        }
    }
    this.surface_timer = setTimeout(this.updateSurface.bind(this), this.metadata.fiv);
}

Consumer.prototype.isUpdate = function(){
    if(this.is_video_tile){
        this.update();
    }
    if(this.renew_surface){
        this.renew_surface = false;
        return true;
    }else{
        return false;
    }
}

Consumer.prototype.update = function(){
    if(this.current_frame !== undefined){
        let tiles = this.current_frame.getMediaArray();
        //this.current_frame = undefined;
        if(this.priv_interval_time === undefined){
            this.priv_interval_time = new Date();
        }
        this.frame_time = ((new Date()).getTime() - this.priv_interval_time.getTime());
        if(this.frame_time > this.metadata.fiv){
            this.frame_time = this.metadata.fiv - 1;
        }
        this.frame_time = this.frame_time / 1000.0;

        let ctx = this.surface.getContext('2d');
        for(let name in tiles){
            if(this.is_video_tile){ tiles[name].currentTime = this.frame_time; };
            let loc = name.split(',');
            let x = parseInt(loc[0]);
            let y = parseInt(loc[1]);
            let mx = (this.metadata.width * x) / this.metadata.tile.value0;
            let my = (this.metadata.height * y) / this.metadata.tile.value1;
            let wx = this.metadata.width / this.metadata.tile.value0;
            let wy = this.metadata.height / this.metadata.tile.value1;
            ctx.drawImage(tiles[name], mx, my, wx, wy);
            if(Parameters.SHOW_GRID == true){
                ctx.strokeStyle = 'rgb(255,255,255)'; 
                ctx.strokeRect(mx, my, wx, wy);
            }
        }
        
        this.renew_surface = true;
    }else{
        console.log("null frame?");
    }
}


Consumer.prototype.setSequenceNumber = function(seq){
    this.current_sequence = seq;
}
