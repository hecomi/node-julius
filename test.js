var Julius = require('./julius.js')
  , julius = new Julius("setting2.jconf")
;

julius.on('speechReady', function() {
	console.log("onSpeechReady");
});

julius.on('speechStart', function() {
	console.log("onSpeechStart");
});

julius.on('speechStop', function() {
	console.log("onSpeechStop");
});

julius.on('start', function() {
	console.log("onStart");
});

julius.on('pause', function() {
	console.log("onPause");
});

julius.on('result', function(str) {
	console.log("onResult", str);
});

julius.on('error', function(str) {
	console.log("onError", str);
});

julius.start();

setTimeout(function(){ julius.reload('setting.jconf'); julius.start(); }, 5000);
// setTimeout(function(){ julius.start(); }, 6000);
// setTimeout(function(){ julius.stop(); }, 9000);
