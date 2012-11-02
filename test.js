var Julius  = require('./julius.js')
  , grammar = new Julius.Grammar()
;

var hour = [], minute = [];
for (var i = 1; i <= 24; ++i, hour.push(i)) ;
for (var i = 0; i <  60; ++i, minute.push(i)) ;
grammar.addSymbol('HOUR',   hour);
grammar.addSymbol('MINUTE', minute);
grammar.add('<HOUR>時<MINUTE>分');
grammar.add('(電気|テレビ|モニタ)を(つけて|消して)');

// 文法のファイル名を指定（省略時は tmp）
grammar.setFileName('gram/test');

// 文法をコンパイル
grammar.mkdfa(function(err, result) {
	if (err) throw err;

	var julius = new Julius("test.jconf");

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

	// 作成した文法ファイルを削除
	// grammar.deleteFiles();
});

// setTimeout(function(){ julius.reload('setting.jconf'); julius.start(); }, 5000);
// setTimeout(function(){ julius.start(); }, 6000);
// setTimeout(function(){ julius.stop(); }, 9000);
