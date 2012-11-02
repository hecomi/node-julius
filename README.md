Voice recognition library using Julius
=============

これは何？
--------------
Julius を用いて簡単に音声認識を JavaScript で実現するための Node.js モジュールです。

インストール
--------------
	$ npm install julius

使い方
--------------
以下のようにコールバック経由で解析結果を取得します。

```javascript
var Julius = require('./julius.js')
  , julius = new Julius("path.to.jconf")
;

// コールバックの登録
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
```

詳細
--------------
その他詳細は Twitter:@hecomi へご質問いただくか、http://d.hatena.ne.jp/hecomi/ をご参照下さい。
