var Julius   = require('./julius.js')
  , grammar  = new Julius.Grammar()
  , iRemocon = require('iRemocon')
  , iremocon = new iRemocon('192.168.0.2')
;

grammar.add('(テレビ|電気)を(つけて|消して)');
var kana = 'あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみめもやゆよらりるれろわを';
for (var i = 0; i < kana.length; ++i) {
	grammar.add(kana[i]);
}

grammar.compile(function(err, result) {
	if (err) throw err;

	julius = new Julius( grammar.getJconf() );
	julius.on('result', function(str) {
		console.log(str);
		if (str.length < 5) return;
		if (str) console.log(str.slice(0, -1).toString() + 'ます');
		switch (str) {
			case 'テレビをつけて' : iremocon.is(1); break;
			case 'テレビを消して' : iremocon.is(2); break;
			case '電気をつけて'   : iremocon.is(3); break;
			case '電気を消して'   : iremocon.is(4); break;
		}
	});

	julius.start();
});

