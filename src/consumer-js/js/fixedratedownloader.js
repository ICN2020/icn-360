/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

 FixedRateDownloader = function(face, name, rttStatistics, mime, callback){
  this.face = face;
  this.name = name;
  this.callback_ = callback;
  this.finalSegmentNumber = undefined;
  this.segmentStore_ = [];
  this.rttStatistics = rttStatistics;
  this.mime = mime;
  this.segmentNumber = 0;
}

FixedRateDownloader.prototype.startDownload = function(){
  for(let i=0; i<Parameters.FIXED_WINDOW_SIZE; i++){
    let interest = new Interest((new Name(this.name)).appendSegment(this.segmentNumber));
    interest.setInterestLifetimeMilliseconds(Parameters.INTEREST_TIMEOUT);
    this.face.expressInterest(interest, this.handleData.bind(this), this.handleTimeout.bind(this), this.handleNack.bind(this));
    this.rttStatistics.afterSendInterest(interest);
    this.segmentNumber++;
  }
}


FixedRateDownloader.prototype.handleData = function(interest, data){
  if(this.finalSegmentNumber === undefined && data.getMetaInfo().getFinalBlockId().getValue().size() > 0){
      this.finalSegmentNumber = data.getMetaInfo().getFinalBlockId().toSegment();
  }
  this.rttStatistics.afterReceiveData(interest, data);

  let segment_no = data.getName().get(-1).toSegment();
  this.segmentStore_[segment_no] = data.getContent().buf();
  
  if(this.finalSegmentNumber < Object.keys(this.segmentStore_).length){
    let sumLength = 0;
    for(let i=0; i<this.segmentStore_.length; i++){
      sumLength += this.segmentStore_[i].byteLength;
    }

    let array = new Uint8Array(sumLength + 1);
    let pos = 0;
    for(let i=0; i<this.segmentStore_.length; i++){
      array.set(this.segmentStore_[i], pos);
      pos += this.segmentStore_[i].byteLength;
    }
    /*let base64Data = btoa(String.fromCharCode.apply(null, array));
    let url = "data:" + this.mime + ";base64," + base64Data; /* */
    let blob = new FileBlob([array.buffer], {'type':this.mime});
    let url = URL.createObjectURL(blob); /* */
    this.callback_(url);
  }else if(this.segmentNumber <= this.finalSegmentNumber){
    let interest = new Interest((new Name(this.name)).appendSegment(this.segmentNumber));
    interest.setInterestLifetimeMilliseconds(Parameters.INTEREST_TIMEOUT);
    this.face.expressInterest(interest, this.handleData.bind(this), this.handleTimeout.bind(this), this.handleNack.bind(this));
    this.rttStatistics.afterSendInterest(interest);
    this.segmentNumber++;
  }
}

FixedRateDownloader.prototype.handleTimeout = function(interest){
  this.rttStatistics.afterTimeout(interest);
  console.log("timeout");  
  this.callback_(undefined);
}

FixedRateDownloader.prototype.handleNack = function(interest, nack){
  this.rttStatistics.afterReceiveNack(interest, nack);
  console.log("nack" + nack.getReason());
  this.callback_(undefined);
}