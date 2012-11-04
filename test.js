var Julius  = require('./julius.js')
  , grammar = new Julius.Grammar()
;

// 文法をセットする
var hour = [], minute = [];
for (var i = 0; i < 24; ++i, hour.push(i)) ;
for (var i = 0; i < 60; ++i, minute.push(i)) ;
grammar.addSymbol('HOUR',   hour);
grammar.addSymbol('MINUTE', minute);
grammar.add('<HOUR>時<MINUTE>分に(電気|テレビ|モニタ)を(つけて|消して)');
grammar.add('終了(して)?');

// 文法のファイル名を指定（省略時は tmp）
grammar.setFileName('tmp');

// 文法をコンパイル
grammar.compile(function(err, result) {
	if (err) throw err;
	console.log(result);

	var julius = new Julius( grammar.getJconf() );

	julius.on('speechReady', function() {
		console.log('onSpeechReady');
	});

	julius.on('speechStart', function() {
		console.log('onSpeechStart');
	});

	julius.on('speechStop', function() {
		console.log('onSpeechStop');
	});

	julius.on('start', function() {
		console.log('onStart');
	});

	julius.on('pause', function() {
		console.log('onPause');
	});

	julius.on('result', function(str) {
		console.log('onResult');
		console.log('認識結果: ' + str);
		if (/終了/.test(str)) {
			julius.stop();
		}
	});

	julius.on('error', function(str) {
		console.log('onError', str);
	});

	setTimeout(function() {
		julius.start();
	}, 1000);

	// 作成した文法ファイルを削除
	grammar.deleteFiles();

	setTimeout(function(){
		julius.stop();
		setTimeout(function() {
			julius.start();
		}, 1000);
	}, 5000);
});

