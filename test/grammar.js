var JuliusGrammar = require('JuliusGrammar')
  , grammar       = new JuliusGrammar()
  , MeCab         = require('mecab-async')
  , mecab         = new MeCab()
  , kana2voca     = require('kana2voca').sync;

// 下記のような正規表現っぽい形式が使えます
grammar.add('(ほげ+|ふが*){2,4}');
grammar.add('ふぅ?');
grammar.add('ばぁ{3}');

// シンボルを定義することもできます
var hour = [], minute = [];
for (var i = 1; i <= 24; ++i, hour.push(i)) ;
for (var i = 0; i <  60; ++i, minute.push(i)) ;
grammar.addSymbol('HOUR',   hour);
grammar.addSymbol('MINUTE', minute);
grammar.add('<HOUR>時(<MINUTE>分)に起こして');

// 文法のファイル名を指定（省略時は tmp）
grammar.setFileName('tmp');

// 文法をコンパイル
grammar.mkdfa(function(err, result) {
	if (err) throw err;
	// 文法をテストする
	grammar.test(function(err, result) {
		if (err) throw err;
		console.log(result.stdout);
		// 作成した文法ファイルを削除
		grammar.deleteFiles();
	});
});
