/*! angular-poller v0.4.2 10-12-2015 */
!function(a,b,c){"use strict";b.module("emguo.poller",[]).constant("pollerConfig",{stopOn:null,resetOn:null,smart:!1,neverOverwrite:!1,handleVisibilityChange:!1}).run(["$rootScope","$document","poller","pollerConfig",function(a,b,c,d){function e(a){return a&&("$stateChangeStart"===a||"$routeChangeStart"===a||"$stateChangeSuccess"===a||"$routeChangeSuccess"===a)}function f(){b[0].hidden?c.delayAll():c.resetDelay()}e(d.stopOn)&&a.$on(d.stopOn,function(){c.stopAll()}),e(d.resetOn)&&a.$on(d.resetOn,function(){c.reset()}),d.handleVisibilityChange&&(f(),b.on("visibilitychange",f))}]).factory("poller",["$interval","$q","$http","pollerConfig",function(a,d,e,f){function g(a,b){this.target=a,this.set(b)}function h(a){var c=null;return f.neverOverwrite||b.forEach(i,function(d){b.equals(d.target,a)&&(c=d)}),c}var i=[],j={action:"get",argumentsArray:[],delay:5e3,idleDelay:1e4,smart:f.smart,catchError:!1};return g.prototype.set=function(a){var c=["action","argumentsArray","delay","idleDelay","smart","catchError"];b.forEach(c,function(b){a&&a[b]?this[b]=a[b]:this[b]||(this[b]=j[b])},this),this.normalDelay=this.delay},g.prototype.start=function(){function c(){(!l||b.isUndefined(f)||f.$resolved)&&(g=new Date,f=h[i].apply(n,j),f.$resolved=!1,(f.$promise||f).then(function(a){f.$resolved=!0,(b.isUndefined(n.stopTimestamp)||g>=n.stopTimestamp)&&n.deferred.notify(a)},function(a){f.$resolved=!0,m&&(b.isUndefined(n.stopTimestamp)||g>=n.stopTimestamp)&&n.deferred.notify(a)}))}var f,g,h=this.target,i=this.action,j=this.argumentsArray.slice(0),k=this.delay,l=this.smart,m=this.catchError,n=this;this.deferred=this.deferred||d.defer(),"string"==typeof h&&(j.unshift(h),h=e),c(),this.interval=a(c,k),this.promise=this.deferred.promise},g.prototype.stop=function(){b.isDefined(this.interval)&&(a.cancel(this.interval),this.interval=c,this.stopTimestamp=new Date)},g.prototype.remove=function(){var a=i.indexOf(this);this.stop(),i.splice(a,1)},g.prototype.restart=function(){this.stop(),this.start()},{get:function(a,b){var c=h(a);return c?(c.set(b),c.restart()):(c=new g(a,b),i.push(c),c.start()),c},size:function(){return i.length},stopAll:function(){b.forEach(i,function(a){a.stop()})},restartAll:function(){b.forEach(i,function(a){a.restart()})},reset:function(){this.stopAll(),i=[]},delayAll:function(){b.forEach(i,function(a){a.delay=a.idleDelay,a.restart()})},resetDelay:function(){b.forEach(i,function(a){a.delay=a.normalDelay,a.restart()})}}}])}(window,window.angular);