/* ------------------------------------------------------------------------- */
// ライブラリの読み込み
/* ------------------------------------------------------------------------- */
var Gin       = require('./gin.js')
  ; path      = require('path')
  ; exec      = require('child_process').exec
  ; fs        = require('fs')
  ; async     = require('async')
  ; MeCab     = require('mecab-async')
  ; mecab     = new MeCab()
;

/* ------------------------------------------------------------------------- */
// カタカナ --> voca 変換
/* ------------------------------------------------------------------------- */
String.prototype.fromKanaToVoca = function() {
	var result = this;
	for (var str in escape_strings) {
		result = result.split(str).join('');
	}
	for (var katakana in katakana_to_hiragana) {
		var hiragana = katakana_to_hiragana[katakana];
		result = result.split(katakana).join(hiragana);
	}
	for (var hiragana in hiragana_to_voca) {
		var voca = hiragana_to_voca[hiragana];
		result = result.split(hiragana).join(voca);
	}
	while (1) {
		var tmp = result.split('::').join(':');
		if (result === tmp) break;
		else result = tmp;
	}
	return result;
};

var escape_strings = [
	'"', '「', '」', '。', '、', ',', '・', '"'
];

var katakana_to_hiragana = {
	'ア' : 'あ',
	'イ' : 'い',
	'ウ' : 'う',
	'エ' : 'え',
	'オ' : 'お',
	'カ' : 'か',
	'キ' : 'き',
	'ク' : 'く',
	'ケ' : 'け',
	'コ' : 'こ',
	'サ' : 'さ',
	'シ' : 'し',
	'ス' : 'す',
	'セ' : 'せ',
	'ソ' : 'そ',
	'タ' : 'た',
	'チ' : 'ち',
	'ツ' : 'つ',
	'テ' : 'て',
	'ト' : 'と',
	'ナ' : 'な',
	'ニ' : 'に',
	'ヌ' : 'ぬ',
	'ネ' : 'ね',
	'ノ' : 'の',
	'ハ' : 'は',
	'ヒ' : 'ひ',
	'フ' : 'ふ',
	'ヘ' : 'へ',
	'ホ' : 'ほ',
	'マ' : 'ま',
	'ミ' : 'み',
	'ム' : 'む',
	'メ' : 'め',
	'モ' : 'も',
	'ヤ' : 'や',
	'ユ' : 'ゆ',
	'ヨ' : 'よ',
	'ヲ' : 'を',
	'ラ' : 'ら',
	'リ' : 'り',
	'ル' : 'る',
	'レ' : 'れ',
	'ロ' : 'ろ',
	'ワ' : 'わ',
	'ン' : 'ん',
	'ヴ' : 'う゛',
	'ァ' : 'ぁ',
	'ィ' : 'ぃ',
	'ゥ' : 'ぅ',
	'ェ' : 'ぇ',
	'ォ' : 'ぉ',
	'ガ' : 'が',
	'ギ' : 'ぎ',
	'グ' : 'ぐ',
	'ゲ' : 'げ',
	'ゴ' : 'ご',
	'ザ' : 'ざ',
	'ジ' : 'じ',
	'ズ' : 'ず',
	'ゼ' : 'ぜ',
	'ゾ' : 'ぞ',
	'ダ' : 'だ',
	'ヂ' : 'ぢ',
	'ヅ' : 'づ',
	'デ' : 'で',
	'ド' : 'ど',
	'バ' : 'ば',
	'ビ' : 'び',
	'ブ' : 'ぶ',
	'ベ' : 'べ',
	'ボ' : 'ぼ',
	'パ' : 'ぱ',
	'ピ' : 'ぴ',
	'プ' : 'ぷ',
	'ペ' : 'ぺ',
	'ポ' : 'ぽ',
	'ャ' : 'ゃ',
	'ュ' : 'ゅ',
	'ョ' : 'ょ'
};

var hiragana_to_voca = {
	'う゛ぁ' : ' b a',
	'う゛ぃ' : ' b i',
	'う゛ぇ' : ' b e',
	'う゛ぉ' : ' b o',
	'う゛ゅ' : ' by u',
	'ぅ゛'   : ' b u',
	'あぁ'   : ' a a',
	'いぃ'   : ' i i',
	'いぇ'   : ' i e',
	'いゃ'   : ' y a',
	'うぅ'   : ' u:',
	'えぇ'   : ' e e',
	'おぉ'   : ' o:',
	'かぁ'   : ' k a:',
	'きぃ'   : ' k i:',
	'くぅ'   : ' k u:',
	'くゃ'   : ' ky a',
	'くゅ'   : ' ky u',
	'くょ'   : ' ky o',
	'けぇ'   : ' k e:',
	'こぉ'   : ' k o:',
	'がぁ'   : ' g a:',
	'ぎぃ'   : ' g i:',
	'ぐぅ'   : ' g u:',
	'ぐゃ'   : ' gy a',
	'ぐゅ'   : ' gy u',
	'ぐょ'   : ' gy o',
	'げぇ'   : ' g e:',
	'ごぉ'   : ' g o:',
	'さぁ'   : ' s a:',
	'しぃ'   : ' sh i:',
	'すぅ'   : ' s u:',
	'すゃ'   : ' sh a',
	'すゅ'   : ' sh u',
	'すょ'   : ' sh o',
	'せぇ'   : ' s e:',
	'そぉ'   : ' s o:',
	'ざぁ'   : ' z a:',
	'じぃ'   : ' j i:',
	'ずぅ'   : ' z u:',
	'ずゃ'   : ' zy a',
	'ずゅ'   : ' zy u',
	'ずょ'   : ' zy o',
	'ぜぇ'   : ' z e:',
	'ぞぉ'   : ' z o:',
	'たぁ'   : ' t a:',
	'ちぃ'   : ' ch i:',
	'つぁ'   : ' ts a',
	'つぃ'   : ' ts i',
	'つぅ'   : ' ts u:',
	'つゃ'   : ' ch a',
	'つゅ'   : ' ch u',
	'つょ'   : ' ch o',
	'つぇ'   : ' ts e',
	'つぉ'   : ' ts o',
	'てぇ'   : ' t e:',
	'とぉ'   : ' t o:',
	'だぁ'   : ' d a:',
	'ぢぃ'   : ' j i:',
	'づぅ'   : ' d u:',
	'づゃ'   : ' zy a',
	'づゅ'   : ' zy u',
	'づょ'   : ' zy o',
	'でぇ'   : ' d e:',
	'どぉ'   : ' d o:',
	'なぁ'   : ' n a:',
	'にぃ'   : ' n i:',
	'ぬぅ'   : ' n u:',
	'ぬゃ'   : ' ny a',
	'ぬゅ'   : ' ny u',
	'ぬょ'   : ' ny o',
	'ねぇ'   : ' n e:',
	'のぉ'   : ' n o:',
	'はぁ'   : ' h a:',
	'ひぃ'   : ' h i:',
	'ふぅ'   : ' f u:',
	'ふゃ'   : ' hy a',
	'ふゅ'   : ' hy u',
	'ふょ'   : ' hy o',
	'へぇ'   : ' h e:',
	'ほぉ'   : ' h o:',
	'ばぁ'   : ' b a:',
	'びぃ'   : ' b i:',
	'ぶぅ'   : ' b u:',
	'ふゃ'   : ' hy a',
	'ぶゅ'   : ' by u',
	'ふょ'   : ' hy o',
	'べぇ'   : ' b e:',
	'ぼぉ'   : ' b o:',
	'ぱぁ'   : ' p a:',
	'ぴぃ'   : ' p i:',
	'ぷぅ'   : ' p u:',
	'ぷゃ'   : ' py a',
	'ぷゅ'   : ' py u',
	'ぷょ'   : ' py o',
	'ぺぇ'   : ' p e:',
	'ぽぉ'   : ' p o:',
	'まぁ'   : ' m a:',
	'みぃ'   : ' m i:',
	'むぅ'   : ' m u:',
	'むゃ'   : ' my a',
	'むゅ'   : ' my u',
	'むょ'   : ' my o',
	'めぇ'   : ' m e:',
	'もぉ'   : ' m o:',
	'やぁ'   : ' y a:',
	'ゆぅ'   : ' y u:',
	'ゆゃ'   : ' y a:',
	'ゆゅ'   : ' y u:',
	'ゆょ'   : ' y o:',
	'よぉ'   : ' y o:',
	'らぁ'   : ' r a:',
	'りぃ'   : ' r i:',
	'るぅ'   : ' r u:',
	'るゃ'   : ' ry a',
	'るゅ'   : ' ry u',
	'るょ'   : ' ry o',
	'れぇ'   : ' r e:',
	'ろぉ'   : ' r o:',
	'わぁ'   : ' w a:',
	'をぉ'   : ' o:',
	'う゛'   : ' b u',
	'でぃ'   : ' d i',
	'でぇ'   : ' d e:',
	'でゃ'   : ' dy a',
	'でゅ'   : ' dy u',
	'でょ'   : ' dy o',
	'てぃ'   : ' t i',
	'てぇ'   : ' t e:',
	'てゃ'   : ' ty a',
	'てゅ'   : ' ty u',
	'てょ'   : ' ty o',
	'すぃ'   : ' s i',
	'ずぁ'   : ' z u a',
	'ずぃ'   : ' z i',
	'ずぅ'   : ' z u',
	'ずゃ'   : ' zy a',
	'ずゅ'   : ' zy u',
	'ずょ'   : ' zy o',
	'ずぇ'   : ' z e',
	'ずぉ'   : ' z o',
	'きゃ'   : ' ky a',
	'きゅ'   : ' ky u',
	'きょ'   : ' ky o',
	'しゃ'   : ' sh a',
	'しゅ'   : ' sh u',
	'しぇ'   : ' sh e',
	'しょ'   : ' sh o',
	'ちゃ'   : ' ch a',
	'ちゅ'   : ' ch u',
	'ちぇ'   : ' ch e',
	'ちょ'   : ' ch o',
	'とぅ'   : ' t u',
	'とゃ'   : ' ty a',
	'とゅ'   : ' ty u',
	'とょ'   : ' ty o',
	'どぁ'   : ' d o a',
	'どぅ'   : ' d u',
	'どゃ'   : ' dy a',
	'どゅ'   : ' dy u',
	'どょ'   : ' dy o',
	'どぉ'   : ' d o:',
	'にゃ'   : ' ny a',
	'にゅ'   : ' ny u',
	'にょ'   : ' ny o',
	'ひゃ'   : ' hy a',
	'ひゅ'   : ' hy u',
	'ひょ'   : ' hy o',
	'みゃ'   : ' my a',
	'みゅ'   : ' my u',
	'みょ'   : ' my o',
	'りゃ'   : ' ry a',
	'りゅ'   : ' ry u',
	'りょ'   : ' ry o',
	'ぎゃ'   : ' gy a',
	'ぎゅ'   : ' gy u',
	'ぎょ'   : ' gy o',
	'ぢぇ'   : ' j e',
	'ぢゃ'   : ' j a',
	'ぢゅ'   : ' j u',
	'ぢょ'   : ' j o',
	'じぇ'   : ' j e',
	'じゃ'   : ' j a',
	'じゅ'   : ' j u',
	'じょ'   : ' j o',
	'びゃ'   : ' by a',
	'びゅ'   : ' by u',
	'びょ'   : ' by o',
	'ぴゃ'   : ' py a',
	'ぴゅ'   : ' py u',
	'ぴょ'   : ' py o',
	'うぁ'   : ' u a',
	'うぃ'   : ' w i',
	'うぇ'   : ' w e',
	'うぉ'   : ' w o',
	'ふぁ'   : ' f a',
	'ふぃ'   : ' f i',
	'ふぅ'   : ' f u',
	'ふゃ'   : ' hy a',
	'ふゅ'   : ' hy u',
	'ふょ'   : ' hy o',
	'ふぇ'   : ' f e',
	'ふぉ'   : ' f o',
	'あ'     : ' a',
	'い'     : ' i',
	'う'     : ' u',
	'え'     : ' e',
	'お'     : ' o',
	'か'     : ' k a',
	'き'     : ' k i',
	'く'     : ' k u',
	'け'     : ' k e',
	'こ'     : ' k o',
	'さ'     : ' s a',
	'し'     : ' sh i',
	'す'     : ' s u',
	'せ'     : ' s e',
	'そ'     : ' s o',
	'た'     : ' t a',
	'ち'     : ' ch i',
	'つ'     : ' ts u',
	'て'     : ' t e',
	'と'     : ' t o',
	'な'     : ' n a',
	'に'     : ' n i',
	'ぬ'     : ' n u',
	'ね'     : ' n e',
	'の'     : ' n o',
	'は'     : ' h a',
	'ひ'     : ' h i',
	'ふ'     : ' f u',
	'へ'     : ' h e',
	'ほ'     : ' h o',
	'ま'     : ' m a',
	'み'     : ' m i',
	'む'     : ' m u',
	'め'     : ' m e',
	'も'     : ' m o',
	'ら'     : ' r a',
	'り'     : ' r i',
	'る'     : ' r u',
	'れ'     : ' r e',
	'ろ'     : ' r o',
	'が'     : ' g a',
	'ぎ'     : ' g i',
	'ぐ'     : ' g u',
	'げ'     : ' g e',
	'ご'     : ' g o',
	'ざ'     : ' z a',
	'じ'     : ' j i',
	'ず'     : ' z u',
	'ぜ'     : ' z e',
	'ぞ'     : ' z o',
	'だ'     : ' d a',
	'ぢ'     : ' j i',
	'づ'     : ' z u',
	'で'     : ' d e',
	'ど'     : ' d o',
	'ば'     : ' b a',
	'び'     : ' b i',
	'ぶ'     : ' b u',
	'べ'     : ' b e',
	'ぼ'     : ' b o',
	'ぱ'     : ' p a',
	'ぴ'     : ' p i',
	'ぷ'     : ' p u',
	'ぺ'     : ' p e',
	'ぽ'     : ' p o',
	'や'     : ' y a',
	'ゆ'     : ' y u',
	'よ'     : ' y o',
	'わ'     : ' w a',
	'ゐ'     : ' i',
	'ゑ'     : ' e',
	'を'     : ' o',
	'ん'     : ' N',
	'っ'     : ' q',
	'ー'     : ':',
	'ぁ'     : ' a',
	'ぃ'     : ' i',
	'ぅ'     : ' u',
	'ぇ'     : ' e',
	'ぉ'     : ' o',
	'ゎ'     : ' w a',
	'ぉ'     : ' o',
	': : :'  : ':',
	': :'    : ':',
	' a a'   : ' a:',
	' i i'   : ' i:',
	' u u'   : ' u:',
	' e e'   : ' e:',
	' e i'   : ' e:',
	' o o'   : ' o:',
	' o u'   : ' o:'
};


/* ------------------------------------------------------------------------- */
// 便利関数
/* ------------------------------------------------------------------------- */
/**
 * 文字列を n 回繰り返す
 * @param[in] num 繰り返し回数
 */
String.prototype.repeat = function( num ) {
	for(var i = 0, buf = ""; i < num; ++i) buf += this;
	return buf;
}

/**
 * 数字を漢字に変換する
 * @param[in] num 整数（１京未満）
 */
Number.prototype.toKanji = function() {
	var num = this;
	if (num === 0) return 'ゼロ';
	if (num >= 10000000000000000) return '無理でした';
	var numKanji    = ['', '一', '二', '三', '四', '五', '六', '七', '八', '九']
	  , ketaKanji   = ['', '十', '百', '千']
	  , kuraiKanji  = ['', '万', '億', '兆']
	  , resultKanji = ''
	;
	if (num < 0) {
		resultKanji += 'マイナス';
		num *= -1;
	}
	// 92
	var keta  = num.toString().length
	  , kurai = 0
	;
	while (num > 0) {
		var k = keta - num.toString().length
		  , x = num%10
		  , c = (k%4 === 0) ? kurai : 0
		;
		if (k%4 === 0) ++kurai;
		if (x === 0) k = 0; // ０のつく桁は省く
		if (k > 0 && x === 1) x = 0; // 一桁目以外は '一' を省く
		resultKanji = numKanji[x] + ketaKanji[k%4] + kuraiKanji[c] + resultKanji;
		num = Math.floor(num/10);
	}
	return resultKanji;
}

/**
 * 文字列をカタカナに変換する
 */
String.prototype.toKana = function() {
	var result = mecab.parseSync( this.toString() )
	  , kana = ''
	;
	for (var i in result) {
		if (!result[i][9]) {
			if (result[i][2] === '数' && !result[i][9]) {
				kana += parseInt(result[i][0]).toKanji().toKana();
			} else {
				kana += result[i][0];
			}
		} else {
			kana += result[i][9];
		}
	}
	return kana;
}

/**
 * 文字列をvoca形式に変換する
 */
String.prototype.toVoca = function() {
	var kana = this.toString().toKana();
	return kana.fromKanaToVoca();
}

/* ------------------------------------------------------------------------- */
// Gin による構文解析
/* ------------------------------------------------------------------------- */

//! Julius の形式に変換するための Grammar
var Voca = new Gin.Grammar({
	Expr     : / ((Group|Symbol|String)(MinMax|Repeat|Plus|Asterisk|Question)?)+ /,
	Group    : / [(]:child Expr ([|]:bros Expr)* [)]:unchild /,
	MinMax   : / [{] $INT:min [,] $INT:max [}] /,
	Repeat   : / [{] $INT:repeat [}] /,
	Plus     : / [+]:plus /,
	Asterisk : / [*]:asterisk /,
	Question : / [?]:question /,
	Symbol   : / [<] $SYMBOL:symbol [>] /,
	String   : / $STRING:string /
}, 'Expr', Gin.SPACE);

//! 文字列ノードのタイプ
const NODE_TYPE = {
	STRING   : 0,
	SYMBOL   : 1
};

//! 文字列ノードの繰り返しタイプ
const REPEAT_TYPE = {
	NONE         : 0, // 繰り返しなし
	MIN_AND_MAX  : 1, // 繰り返しの最小/最大数を設定
	ONE_OR_MORE  : 2, // ０回以上の繰り返し
	ZERO_OR_MORE : 3  // １回以上の繰り返し
};

/**
 * 文字列ノードクラス.
 * 各ノードの情報を格納する（e.g. 繰り返し回数、次のノード、子ノード）
 */
function Node() {
	this.str    = '';
	this.id     = '';
	this.repeat = REPEAT_TYPE.NONE;
	this.type   = NODE_TYPE.STRING;
	this.parent = null;
	this.child  = null;
	this.next   = null;
	this.min    = -1;
	this.max    = -1;
	this.isNextBros   = false;
}

/**
 * Gin の Semantic Action を引き受けるハンドラ.
 */
var Handler = function() {
	//! 最初のノード位置
	this.first = new Node();

	//! 現在のノード位置
	this.node = this.first;
}
Handler.prototype = {
	//! 現在のノード位置 or 次の位置へ文字列ノードを追加
	string: function(v) {
		if (this.node.str !== '' || this.node.child !== null) {
			this.node.next = new Node();
			this.node.next.parent = this.node.parent;
			this.node = this.node.next;
		}
		this.node.str = v;
	},
	//! 現在のノード位置 or 次の位置へ数字ノードを追加
	symbol: function(v) {
		if (this.node.str != '' || this.node.child != null) {
			this.node.next = new Node();
			this.node.next.parent = this.node.parent;
			this.node = this.node.next;
		}
		this.node.str  = v;
		this.node.type = NODE_TYPE.SYMBOL;
	},
	//! 最小繰り返し回数を設定
	min: function(v) {
		this.node.repeat = REPEAT_TYPE.MIN_AND_MAX;
		this.node.min = v;
	},
	//! 最大繰り返し回数を設定
	max: function(v) {
		this.node.repeat = REPEAT_TYPE.MIN_AND_MAX;
		this.node.max = v;
	},
	//! 固定繰り返し回数を設定
	repeat: function(v) {
		this.node.repeat = REPEAT_TYPE.MIN_AND_MAX;
		this.node.min = this.node.max = v;
	},
	//! １回以上繰り返しに設定
	plus: function(v) {
		this.node.repeat = REPEAT_TYPE.ONE_OR_MORE;
	},
	//! ０回以上繰り返しに設定
	asterisk: function(v) {
		this.node.repeat = REPEAT_TYPE.ZERO_OR_MORE;
	},
	//! ０回か１回出現に設定
	question: function(v) {
		this.node.repeat = REPEAT_TYPE.MIN_AND_MAX;
		this.node.min = 0;
		this.node.max = 1;
	},
	//! 自分の次のノードが兄弟ノードかどうかを記憶
	bros: function(v) {
		this.node.isNextBros = true;
	},
	//! 子要素を設定し現在のノード位置を子ノードへ移動
	child: function(v) {
		this.node.next = new Node();
		this.node.next.parent = this.node.parent;
		this.node.next.child = new Node();
		this.node.next.child.parent = this.node.next;
		this.node = this.node.next.child;
	},
	//! 現在のノード位置を親ノードへ移動
	unchild: function(v) {
		this.node = this.node.parent;
	}
};


/**
 * Julius の文法に必要なファイルを生成する
 */
var JuliusData = function() {
	this.num_             = 0;
	this.DEFAULT_VOCA_STR = '% NS_B\n<s>\tsilB\n% NS_E\n<s>\tsilE\n% NOISE\n<sp>\tsp\n';
	this.DEFAULT_GRAM_STR = 'S\t: NS_B NOISE NS_E\n';
	this.voca_            = this.DEFAULT_VOCA_STR;
	this.grammar_         = this.DEFAULT_GRAM_STR;
	this.fileName_        = 'tmp';
	this.mkdfaPath_       = path.join(__dirname, 'tool/mkdfa');
	this.generatePath_    = path.join(__dirname, 'tool/generate');
}

JuliusData.prototype = {
	/**
	 * Gin による構文解析結果から Julius の grammar 形式、voca 形式を生成する.
	 * 解析結果（Nodeクラス）は兄弟/子供を持つので、再帰的に子供を調べる
	 *
	 * @param[in] firstNum      grammar や voca で用いる ID 番号
	 * @param[in] firstNode     Gin によって解析された結果のノード
	 * @param[in] parentId      (再帰で使用) 親の ID. （e.g. WORD_5 など）
	 * @return                  {grammar, voca, num} 構造体
	 */
	makeJuliusFormat: function (firstNum, firstNode, parentId) {
		var num = firstNum;
		var gramStr = '', vocaStr = '';

		// 最上位ノードの場合
		if (parentId === undefined) {
			// ルートとなる文法を作成する
			// 繰り返し用に最上位ノードの場合は ROOT_N --> WORD_N という対応付けをする
			var rootGramStr = '';
			gramStr += 'S\t: NS_B ';
			for (var node = firstNode, n = firstNum; node; node = node.next) {
				if (node.child !== null || node.str !== '') {
					rootGramStr += 'ROOT_' + n + '\t: WORD_' + n + '\n';
					gramStr += 'ROOT_' + n + ' ';
					++n;
				}
			}
			gramStr += 'NS_E\n';
			gramStr += rootGramStr;
		}

		// ノードを順に走査（next）
		for (var node = firstNode; node; node = node.next) {
			// 子ノードがいないかつ空ノード（頭とか）は無視
			if (node.child === null && node.str === '') continue;

			// 自身の ID を設定 (最上位ノードかどうかで場合分け）
			var id, parentId2;
			if (parentId === undefined) {
				parentId2 = 'ROOT_' + num;
				id = 'WORD_' + num;
			} else {
				parentId2 = parentId;
				id = parentId + '_' + num;
			}

			// 繰り返しに対応する grammar を作る
			var loopId = id + '_LOOP'; tmpId = id + '_TMP';
			switch (node.repeat) {
				case REPEAT_TYPE.NONE:
					break;
				case REPEAT_TYPE.MIN_AND_MAX:
					for (var i = node.min; i <= node.max; ++i) {
						if (i === 0)
							gramStr += id + '\t: NOISE\n';
						else
							gramStr += id + '\t: ' + (loopId + ' ').repeat(i) + '\n';
					}
					id = loopId;
					break;
				case REPEAT_TYPE.ZERO_OR_MORE:
					gramStr += id + '\t: NOISE\n';
					gramStr += id + '\t: ' + loopId + '\n';
					gramStr += id + '\t: ' + id + ' ' + loopId + '\n';
					id = loopId;
					break;
				case REPEAT_TYPE.ONE_OR_MORE:
					gramStr += id + '\t: ' + loopId + '\n';
					gramStr += id + '\t: ' + id + ' ' + loopId + '\n';
					id = loopId;
					break;
				default:
					throw new Error("ERROR! Invalid REPEAT_TYPE.");
					break;
			}

			// 再帰処理
			// 子ノード（= child）がいるとき（= 自分の str は空）、子ノードを走査
			if (node.child !== null) {
				// 文法を作成
				// isNextBros が設定されているノードの時はそこの位置がセパレータとなる
				gramStr += id + '\t: ';
				for (var child = node.child, m = 0; child; child = child.next, ++m) {
					gramStr += id + '_' + m + ' ';
					if (child.isNextBros === true) {
						gramStr += '\n' + id + '\t: ';
					}
				}
				gramStr += '\n';

				var result;
				// 親IDに自分のIDをひもづける
				result = this.makeJuliusFormat(0, node.child, id);
				gramStr += result.grammar;
				vocaStr += result.voca;
				++num;
			}


			// 子ノードがいないが空でないノードの場合(= 終端ノード)は voca を書きだして次へ
			if (node.child === null && node.str !== '') {
				// MeCab と ICU を用いて Julius の voca 形式に変換
				// Symbol なら voca 形式に登録せずに grammar に追加
				switch (node.type) {
					case NODE_TYPE.STRING:
						vocaStr +=
							'% ' + id + '\n' +
							node.str + '\t' + node.str.toVoca() + '\n';
						break;
					case NODE_TYPE.SYMBOL:
						gramStr += id + '\t: ' + node.str + '\n';
						break;
					default:
						throw new Error("ERROR! Invalid NODE_TYPE.");
						break;
				}
				++num;
			}

		}
		return {grammar: gramStr, voca: vocaStr, num: num};
	},

	/**
	 * Julius が認識することのできる文字列を追加
	 * @param[in] str 追加する表現
	 */
	add: function(str) {
		var handler = new Handler();
		var match   = Voca.parse(str, handler);

		if (match && match.full) {
			var result     = this.makeJuliusFormat(this.num_, handler.first);
			this.voca_    += result.voca;
			this.grammar_ += result.grammar;
			this.num_      = result.num;
		} else {
			throw new Error('ERROR! "' + str + '" is invalid format.');
		}
	},

	/**
	 * symbol を追加.
	 * @param[in] symbol 追加するシンボル
	 * @param[in] arr    シンボルに対応する文字列配列
	 * @param[in] sArr   arr を ['一', '二']、sArr を ['1', '2'] として渡すと voca が '1 i ch i \n 2 n i' となる。
	 */
	addSymbol: function(symbol, arr) {
		if (!/[a-zA-Z0-9_-]/.test(symbol)) {
			throw new Error('ERROR! "' + symbol + '" is invalid symbol.');
		}
		this.voca_ += '% ' + symbol + '\n';
		for (var i in arr) {
			var str     = arr[i].toString();
			this.voca_ += str + '\t' + str.toVoca() + '\n';
		}
	},

	/**
	 * voca と grammar をリセットする
	 */
	reset : function() {
		this.voca_    = this.DEFAULT_VOCA_STR;
		this.grammar_ = this.DEFAULT_GRAM_STR;
	},

	/**
	 * voca / grammar / dfa / dict / term を削除する
	 * @param[in] callback 処理が終了した時に実行されるコールバック
	 */
	deleteFiles : function(callback) {
		var command = 'rm ';
		['.voca', '.grammar', '.dfa', '.dict', '.term', '.jconf'].forEach(function(ext) {
			command += this.fileName_ + ext + ' ';
		}.bind(this));
		exec(command, function(err, stdout, stderr) {
			if (err)    next(err,    null);
			if (stderr) next(stderr, null);
			if ( typeof(callback) == 'function' ) callback(null, stdout);
		});
	},

	/**
	 * 出力するファイル名を出力
	 * @param[in] fileName 出力ファイル名
	 */
	getJconf : function() {
		return this.fileName_ + '.jconf';
	},

	/**
	 * 出力するファイル名を出力
	 * @param[in] fileName 出力ファイル名
	 */
	setFileName: function(fileName) {
		this.fileName_ = fileName;
	},

	/**
	 * voca / grammar ファイルを書き出して mkdfa を実行し、
	 * コンフィグファイル（.jconf）を作成する
	 * @param[in] callback 処理が終了した時に実行されるコールバック
	 */
	compile: function(callback) {
		async.series({
			voca: function(next) {
				var fileName = this.fileName_ + '.voca';
				fs.writeFile(fileName, this.voca_, next);
			}.bind(this),
			grammar: function(next) {
				var fileName = this.fileName_ + '.grammar';
				fs.writeFile(fileName, this.grammar_, next);
			}.bind(this),
			mkdfa: function(next) {
				var command = this.mkdfaPath_ + ' ' + this.fileName_;
				exec(command, function(err, stdout, stderr) {
					if (err) {
						next(err, null);
						return;
					}
					var result = {stderr: stderr, stdout: stdout};
					next(null, result);
				});
			}.bind(this),
			compile : function(next) {
				var fileName = this.fileName_ + '.jconf';
				var jconf    = '';
				jconf += '-input mic\n';
				jconf += '-gram ' + this.fileName_ + '\n';
				jconf += '-h ' + path.join(__dirname, 'model/phone_m/hmmdefs_monof_mix16_gid.binhmm');
				fs.writeFile(fileName, jconf, function(err) {
					next(err, null);
				});
			}.bind(this)
		},
		function(err, result) {
			if ( typeof(callback) == 'function' ) {
				callback(err, result.mkdfa);
			}
		});
	},

	/**
	 * 生成したファイルを generate を用いてテストする
	 */
	test: function(callback) {
		var command = this.generatePath_ + ' ' + this.fileName_;
		exec(command, function(err, stdout, stderr) {
			if (err) {
				callback(err, null);
				return;
			}
			var result = {stderr: stderr, stdout: stdout};
			if ( typeof(callback) == 'function' ) {
				callback(null, result);
			}
		});
	}
};

/* ------------------------------------------------------------------------- */
// エクスポート
/* ------------------------------------------------------------------------- */
module.exports = JuliusData;
