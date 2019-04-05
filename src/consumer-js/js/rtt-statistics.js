/**
 * icn-360 - 360 degree video streaming producer/consumer on NDN
 * 
 * Copyright (c) 2019 KDDI Research, Inc.
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

RttStatistics = function(){
    this.nSent = 0;
    this.nReceived = 0;
    this.nTimeout = 0;
    this.nNacked = 0;
    this.nMarked = 0;
    this.sumRtt = 0;
    this.minRtt = Number.MAX_SAFE_INTEGER;
    this.maxRtt = 0;
    // This works when the name of interest and data are exactly the same 
    this.timeContainer = {};
    this.canSchedule = true;
}

RttStatistics.prototype.printStatus = function(){
    
    let strout = this.nSent + " pkts transmitted, " + this.nReceived + " received<br>" + this.nTimeout + " timeout (" + this.nNacked + "/" + this.nMarked + " Nack/Mark, " + (100*(this.nSent-this.nReceived)/this.nSent).toFixed(3) + "% lost)<br>RTT avg/min/max = " + (this.sumRtt/this.nReceived).toFixed(3) + "/" + this.minRtt + "/" + this.maxRtt + "(ms)";
    document.getElementById('i360-status').innerHTML = strout;
    this.canSchedule = true;
}

RttStatistics.prototype.afterSendInterest = function(interest){
    this.nSent += 1;
    now = + new Date();
    this.timeContainer[interest.getName().toUri()] = now;
}
RttStatistics.prototype.afterReceiveData = function(interest, data){
    this.nReceived += 1;
    now = + new Date();
    name = interest.getName().toUri();
    let diff = now - this.timeContainer[name];
    delete this.timeContainer[name];
    this.appendRtt(diff);
    if(data.getCongestionMark()	!= 0){
        this.nMarked += 1;
    }
}
RttStatistics.prototype.afterTimeout = function(interest){
    this.nTimeout += 1;
    delete this.timeContainer[interest.getName().toUri()];
}
RttStatistics.prototype.afterReceiveNack = function(interest, nack){
    this.nNacked += 1;
    delete this.timeContainer[interest.getName().toUri()];
}

RttStatistics.prototype.appendRtt = function(rtt){
    this.sumRtt += rtt;
    if (this.minRtt > rtt){
        this.minRtt = rtt;
    }
    if (this.maxRtt < rtt){
        this.maxRtt = rtt;
    }
    if (this.canSchedule){
	this.canSchedule = false;
 	setTimeout(this.printStatus.bind(this), 6000);
    }
}