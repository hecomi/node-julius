var Julius  = require('./julius.js')
  , grammar = new Julius.Grammar()
;

// 音声認識させる言葉を覚えさせる
grammar.add('おはようございます');
grammar.add('こんにちは');
grammar.add('おやすみなさい');

// 登録したキーワードをコンパイル
grammar.compile(function(err, result) {
	if (err) throw err;
	// Julius を実行する
	julius = new Julius( grammar.getJconf() );

	julius.on('result', function(str) {
		console.log(str);
	});

	julius.start();
});

