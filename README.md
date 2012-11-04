Node-Julius : Easy voice recognition library using Julius
=============

これは何？
--------------
フリーの大語彙連続音声認識システム Julius を用いて簡単に音声認識を JavaScript で実現するための Node.js モジュールです。

動作環境
--------------
Mac OS X 10.8.2 での動作を確認しています。Ubuntu 10.04 では ICU4C 関連のエラーが残っていますが一応動作します。
コンパイルには以下のものが必要です。（）内はこちらで動作させたバージョンになります。

+ C++11 対応のコンパイラ （Mac 付属の Apple clang version 4.0 / g++-4.7）
+ Boost （1.49.0 / 1.50.0）
+ ICU4C （49.1.2 / 4.2（一部メモリリーク？によるバグ有り））
+ MeCab （0.994）

インストール
--------------
	$ cd YOUR_NODE_PROJECT_DIR
	$ git clone https://github.com/hecomi/node-julius
	$ mkdir node_modules
	$ mv node-julius node_modules/julius
	$ cd node_modules/julius
	$ make

使い方
--------------
Grammar で音声認識させる言葉を覚えさせ、これをもとに Julius を実行する形式となります。
下記コードで、おはようございます・こんにちは・おやすみなさい、を音声認識することができます。

```javascript
var Julius  = require('./julius.js')
  , grammar = new Julius.Grammar()
;

// 音声認識させる言葉を覚えさせる
grammar.add('おはようございます');
grammar.add('こんにちは');
grammar.add('おやすみなさい');

// 登録したキーワードをコンパイルして Julius を実行する
grammar.compile(function(err, result) {
	if (err) throw err

	// Julius インスタンスの生成
	var julius = new Julius( grammar.getJconf() );

	// 認識結果のコールバックを追加
	julius.on('result', function(str) {
		console.log(str);
	});

	// 認識の開始
	julius.start();
});
```

文法の登録には、下記のように正規表現のような記法が可能です。
また、シンボルを定義して時間のような数字のまとまり等を一括で登録することもできます。

```javascript
// 下記のような正規表現っぽい形式が使えます
grammar.add('(ほげ+|ふが*){2,4}');
grammar.add('ふぅ?');
grammar.add('ばぁ{3}');

// シンボルを定義することもできます
// 下記は「○○時」および「○○時○○分」を認識します。
var hour = [], minute = [];
for (var i = 1; i <= 24; ++i, hour.push(i)) ;
for (var i = 0; i <  60; ++i, minute.push(i)) ;
grammar.addSymbol('HOUR',   hour);
grammar.addSymbol('MINUTE', minute);
grammar.add('<HOUR>時(<MINUTE>分)?');

// 文法のファイル名を指定（省略時は tmp）
grammar.setFileName('test');

// 文法をコンパイル
grammar.compile(function(err, result) {
	if (err) throw err;
	// 文法をテストする
	grammar.test(function(err, result) {
		if (err) throw err;
		console.log(result.stdout);
		// 作成した文法ファイルを削除
		// grammar.deleteFiles();
	});
});
```

音声認識では、各種イベントを受け取るコールバックを定義することができます。

```javascript
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
```

認識の一時停止、開始は `start` および `stop` で行います。

```javascript
setTimeout(function(){
	julius.stop();
	setTimeout(function() {
		julius.start();
	}, 5000);
}, 5000);
```

詳細
--------------
その他詳細は Twitter:@hecomi へご質問いただくか、http://d.hatena.ne.jp/hecomi/ をご参照下さい。
