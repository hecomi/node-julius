node-execsync
==============

はじめに
--------------
node.js の exec の同期版です。
shell のコマンドを引数で指定して実行して、その実行結果を戻り値で返してくれます。

ライセンス
--------------
NYSL ライセンスです。

インストール
--------------
以下のコマンドを実行して下さい。

```
$ npm install execsync
```

使い方
--------------
次のように使います。

```javascript
var execSync = require('execsync');
console.log(execSync('ls'));
```

詳細
--------------
その他詳細は Twitter:@hecomi へご質問いただくか、http://d.hatena.ne.jp/hecomi/ をご参照下さい。

