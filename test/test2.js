var Julius   = require('../julius')
  , grammar  = new Julius.Grammar()
;

grammar.add('(テレビ|電気)を(つけて|消して)');

// ゴミワードを加える
var kana = 'あいうえお';
for (var i = 0; i < kana.length; ++i) {
	for (var j = 0; j < kana.length; ++j) {
		for (var k = 0; k < kana.length; ++k) {
			grammar.add(kana[i] + kana[j] + kana[k]);
		}
	}
}

var remocon = function(num) {
	switch (num) {
		case 1: console.log('turn on TV');     break;
		case 2: console.log('turn off TV');    break;
		case 3: console.log('turn on light');  break;
		case 4: console.log('turn off light'); break;
	}
};

grammar.compile(function(err, result) {
	if (err) throw err;

	julius = new Julius( grammar.getJconf() );
	grammar.deleteFiles();

	julius.on('result', function(str) {
		console.log(str);
		if (str.length < 5) return;
		if (str) console.log(str.slice(0, -1).toString() + 'ます');
		switch (str) {
			case 'テレビをつけて' : remocon(1); break;
			case 'テレビを消して' : remocon(2); break;
			case '電気をつけて'   : remocon(3); break;
			case '電気を消して'   : remocon(4); break;
		}
	});

	julius.start();
});

