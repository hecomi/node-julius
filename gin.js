/*
 * Gin - a recursive descent parser generator
 *
 * Copyright (c) 1998-2003 Joel de Guzman
 * Copyright (c) 2001 Daniel Nuffer
 * Copyright (c) 2002 Hartmut Kaiser
 * Copyright (c) 2003 Martin Wille
 * http://spirit.sourceforge.net/
 * Copyright (c) 2007 nanto_vi (TOYAMA Nao) <nanto@moon.email.ne.jp>
 *
 * Use, modification and distribution is subject to the Boost Software
 * License, Version 1.0 <http://www.boost.org/LICENSE_1_0.txt> or the
 * MIT License <http://www.opensource.org/licenses/mit-license.php>.
 *
 */

if (!Gin)
  var Gin = {};

Gin.VERSION = "0.90";
Gin.BUILD = 20070912;


// ==== Utils ====

Gin.Utils = {
  inherit: function (Sub, Super, members) {
    function Traits() {}
    Traits.prototype = Super.prototype;
    Sub.prototype = new Traits();
    Sub.prototype.constructor = Sub;
    Sub.prototype._super = function () {
      var original = this._super;
      this._super = Super.prototype._super || null;
      Super.apply(this, arguments);
      if (this.constructor == Sub)
        delete this._super;
      else
        this._super = original;
    };
    Gin.Utils.extend(Sub.prototype, members);
  },
  extend: function (destination, source) {
    for (var i in source)
      destination[i] = source[i];
    return destination;
  },
  makeTuple: function () {
    var tuple = [];
    tuple.isTuple = true;
    for (var i = 0; i < arguments.length; i++) {
      var value = arguments[i];
      if (value instanceof Array && value.isTuple && !value.ruleName)
        tuple.push.apply(tuple, value);
      else
        tuple.push(value);
    }
    return tuple;
  },
  quote: function (string) {
    return '"' + Gin.Utils.escape(string) + '"';
  },
  unquote: function (string) {
    return Gin.Utils.unescape(String(string).slice(1, -1));
  },
  escape: (function () {
    var map = { "\0": "x00", "\b": "b", "\f": "f", "\n": "n", "\r": "r",
                "\t": "t", "\v": "v", "\"": "\"", "\\": "\\" };
    function fn(c) { return "\\" + map[c]; }
    return function (string) {
      return String(string).replace(/[\0\b\f\n\r\t\v"\\]/g, fn);
    };
  })(),
  unescape: (function () {
    var map = { b: "\b", f: "\f", n: "\n", r: "\r", t: "\t", v: "\v" };
    function fn(match, any, control, code, octal) {
      return control ? map[control] :
             code ? String.fromCharCode("0x" + code.substring(1)) :
             octal ? String.fromCharCode(parseInt(octal, 8)) : any;
    }
    return function (string) {
      return String(string).replace(
        /\\(([bfnrtv])|(u[0-9A-Fa-f]{4}|x[0-9A-Fa-f]{2})|([0-7]{1,3})|.)/g, fn
      );
    };
  })()
};


// ==== Parser ====

Gin.Parser = function () {
  throw new Gin.Error(Gin.Error.NOT_IMPLEMENTED);
};
Gin.Parser.prototype.parse =
function (scanner, handler) {
  throw new Gin.Error(Gin.Error.NOT_IMPLEMENTED);
};

Gin.Parser.String = function (string) {
  this._value = String(string);
  this._length = this._value.length;
};
Gin.Utils.inherit(Gin.Parser.String, Gin.Parser);
Gin.Parser.String.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner);
  var start = scanner.getIndex();
  if (scanner.substring(0, this._length) != this._value) {
    scanner.setIndex(start);
    return null;
  }
  scanner.moveIndex(this._length);
  return new Gin.Match(this._value, scanner);
};

Gin.Parser.RegExp = function (pattern, flags) {
  if (pattern instanceof RegExp) {
    this._global = pattern.global;
    this._value = pattern;
    flags = (pattern.ignoreCase ? "i" : "") + (pattern.multiline ? "m" : "");
    pattern = pattern.source;
  } else {
    flags = String(flags || "");
    this._global = (flags.indexOf("g") != -1);
    this._value = new RegExp(pattern, flags);
    flags = flags.replace("g", "");
  }
  this._re = new RegExp("^(?:" + pattern + ")", flags);
};
Gin.Utils.inherit(Gin.Parser.RegExp, Gin.Parser);
Gin.Parser.RegExp.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner);
  var start = scanner.getIndex();
  var match = scanner.match(this._re);
  if (!match) {
    scanner.setIndex(start);
    return null;
  }
  var value = new String(match[0]);
  value.match = match;
  scanner.moveIndex(value.length);
  return new Gin.Match(value, scanner);
};

Gin.Parser.Ref = function (base, name) {
  this._base = base;
  this._name = name;
};
Gin.Utils.inherit(Gin.Parser.Ref, Gin.Parser);
Gin.Parser.Ref.prototype.parse =
function (scanner, handler) {
  return this._base[this._name].parse(scanner, handler);
};

Gin.Parser.Nothing = function () {};
Gin.Utils.inherit(Gin.Parser.Nothing, Gin.Parser);
Gin.Parser.Nothing.prototype.parse =
function (scanner, handler) {
  return null;
};

Gin.Parser.End = function () {};
Gin.Utils.inherit(Gin.Parser.End, Gin.Parser);
Gin.Parser.End.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner);
  return scanner.atEnd() ? new Gin.Match("", scanner) : null;
};

Gin.Parser.Unary = function (parser) {
  this._subject = parser;
};
Gin.Utils.inherit(Gin.Parser.Unary, Gin.Parser);

Gin.Parser.Action = function (parser, action) {
  this._super(parser);
  this._action = action;
  this._isRef = (typeof action != "function");
};
Gin.Utils.inherit(Gin.Parser.Action, Gin.Parser.Unary);
Gin.Parser.Action.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner);
  var start = scanner.getIndex();
  var match = this._subject.parse(scanner, handler);
  if (!match) return null;
  var action = this._isRef ? handler[this._action] : this._action;
  var thisObject = this._isRef ? handler : null;
  var value;
  try {
    value = action.call(thisObject, match.value);
  } catch (ex) {
    if ((ex instanceof Gin.Error && ex.message == Gin.Error.NO_MATCH) ||
        ex == Gin.Error.NO_MATCH) {
      scanner.setIndex(start);
      return null;
    }
    throw ex;
  }
  return (value === undefined) ? match : new Gin.Match(value, scanner);
};

Gin.Parser.Repeat = function (parser, min, max) {
  this._super(parser);
  this._min = min;
  this._max = max;
};
Gin.Utils.inherit(Gin.Parser.Repeat, Gin.Parser.Unary);
Gin.Parser.Repeat.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner);
  var start = scanner.getIndex();
  var value = Gin.Utils.makeTuple();
  var count = 0;
  for (var match;
       count < this._max && (match = this._subject.parse(scanner, handler));
       count++)
    value = Gin.Utils.makeTuple(value, match.value);
  if (count < this._min) {
    scanner.setIndex(start);
    return null;
  }
  return new Gin.Match(value, scanner);
};

Gin.Parser.Binary = function (parser1, parser2) {
  this._left = parser1;
  this._right = parser2;
};
Gin.Utils.inherit(Gin.Parser.Binary, Gin.Parser);

Gin.Parser.Seq = function (parser1, parser2) {
  this._super(parser1, parser2);
};
Gin.Utils.inherit(Gin.Parser.Seq, Gin.Parser.Binary);
Gin.Parser.Seq.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner);
  var start = scanner.getIndex();
  var match1 = this._left.parse(scanner, handler);
  if (!match1) return null;
  var match2 = this._right.parse(scanner, handler);
  if (!match2) {
    scanner.setIndex(start);
    return null;
  }
  var value = Gin.Utils.makeTuple(match1.value, match2.value);
  return new Gin.Match(value, scanner);
};

Gin.Parser.Diff = function (parser1, parser2) {
  this._super(parser1, parser2);
};
Gin.Utils.inherit(Gin.Parser.Diff, Gin.Parser.Binary);
Gin.Parser.Diff.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner);
  var start = scanner.getIndex();
  var match1 = this._left.parse(scanner, handler);
  if (!match1) return null;
  scanner.setIndex(start);
  var match2 = this._right.parse(scanner, handler);
  if (match2 && match1.lastIndex <= match2.lastIndex) {
    scanner.setIndex(start);
    return null;
  }
  scanner.setIndex(match1.lastIndex);
  return match1;
};

Gin.Parser.Alt = function (parser1, parser2) {
  this._super(parser1, parser2);
};
Gin.Utils.inherit(Gin.Parser.Alt, Gin.Parser.Binary);
Gin.Parser.Alt.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner);
  return this._left.parse(scanner, handler) ||
         this._right.parse(scanner, handler);
};


// ==== Rule ====

Gin.Rule = function (rule, action, rules, name) {
  this._parser =
    (rule instanceof Gin.Parser) ? rule :
    (rule instanceof RegExp) ? Gin.Rule.Parser.parse(rule, rules) :
    new Gin.Parser.String(rule);
  this._name = name || "";
  var parser = this;
  if (this._parser instanceof Gin.Rule.Action) {
    var ruleAction = this._parser.action;
    parser._parser = this._parser.parser;
    parser = new Gin.Parser.Action(parser, ruleAction);
  }
  if (action)
    parser = new Gin.Parser.Action(parser, action);
  return parser;
};
Gin.Utils.inherit(Gin.Rule, Gin.Parser);
Gin.Rule.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner);
  var match = this._parser.parse(scanner, handler);
  if (!match) return null;
  var value = Gin.Utils.makeTuple(match.value);
  value.ruleName = this._name;
  return new Gin.Match(value, scanner);
};

Gin.Rule.Action = function (parser, action) {
  this.parser = parser;
  this.action = action;
};

Gin.Rule.Parser = {
  parse: function (input, rules) {
    if (input instanceof RegExp)
      input = input.source;
    var tokens = new Gin.Rule.Tokenizer(input);
    return this.parseRuleAction(tokens, rules);
  },
  parseRuleAction: function (tokens, rules) {
    var parser = this.parseAlt(tokens, rules);
    var token = tokens.get();
    if (!token) return parser;
    if (token != "::" || !(token = tokens.get()) ||
        token.type != "ident" || tokens.peek())
      throw new Gin.Error(Gin.Error.INVALID_RULE);
    return new Gin.Rule.Action(parser, token);
  },
  parseAlt: function (tokens, rules) {
    var parser = this.parseDiff(tokens, rules);
    while (tokens.peek() == "|") {
      tokens.get();
      parser = new Gin.Parser.Alt(parser, this.parseDiff(tokens, rules));
    }
    return parser;
  },
  parseDiff: function (tokens, rules) {
    var parser = this.parseSeq(tokens, rules);
    if (tokens.peek() == "-") {
      tokens.get();
      parser = new Gin.Parser.Diff(parser, this.parseSeq(tokens, rules));
    }
    return parser;
  },
  parseSeq: function (tokens, rules) {
    var parser = this.parseRepeat(tokens, rules);
    var token
    while ((token = tokens.peek()) &&
           (token.type == "ident" || token.type == "string" ||
            token.type == "regexp" || token.type == "predef" || token == "("))
      parser = new Gin.Parser.Seq(parser, this.parseRepeat(tokens, rules));
    return parser;
  },
  parseRepeat: function (tokens, rules) {
    var parser = this.parseAction(tokens, rules);
    var token = tokens.peek();
    if (token && token.type == "repeat") {
      tokens.get();
      var min, max;
      if (token == "*" || token == "+" || token == "?") {
        min = (token == "+") ? 1 : 0;
        max = (token == "?") ? 1 : Infinity;
      } else {
        var match = token.match(/(\d+)(,(\d*))?/);
        min = +match[1];
        max = match[3] ? +match[3] : match[2] ? Infinity : min;
      }
      parser = new Gin.Parser.Repeat(parser, min, max);
    }
    return parser;
  },
  parseAction: function (tokens, rules) {
    var parser = this.parsePrimary(tokens, rules);
    while (tokens.peek() == ":") {
      tokens.get();
      var token = tokens.get();
      if (!token || token.type != "ident")
        throw new Gin.Error(Gin.Error.INVALID_RULE);
      parser = new Gin.Parser.Action(parser, token);
    }
    return parser;
  },
  parsePrimary: function (tokens, rules) {
    var token = tokens.get();
    switch (token.type) {
      case "ident":
        if (!rules || !(token in rules))
          throw new Gin.Error(Gin.Error.UNDEFINED_SYMBOL);
        return new Gin.Parser.Ref(rules, token);
      case "string":
        var string = Gin.Utils.unquote(token)
        return new Gin.Parser.String(string);
      case "regexp":
        var delimiterIndex = token.lastIndexOf(">");
        var pattern = token.substring(1, delimiterIndex);
        var flags = token.substring(delimiterIndex + 1);
        return new Gin.Parser.RegExp(pattern, flags);
      case "predef":
        var symbol = token.substring(1);
        if (!(symbol in Gin))
          throw new Gin.Error(Gin.Error.UNDEFINED_SYMBOL);
        return new Gin.Parser.Ref(Gin, symbol);
      case "(":
        var parser = this.parseAlt(tokens, rules);
        if (tokens.get() != ")") break;
        return parser;
    }
    //while ((token = tokens.get())) print(token);
    throw new Gin.Error(Gin.Error.INVALID_RULE);
  }
};

Gin.Rule.Tokenizer = function (input) {
  this._input = String(input);
  this._next = null;
}
Gin.Rule.Tokenizer.prototype.get =
function () {
  var token = this.peek();
  this._next = null;
  return token;
};
Gin.Rule.Tokenizer.prototype.peek =
function () {
  if (this._next) return this._next;
  this._input = this._input.replace(/^\s+/, "");
  if (!this._input) return null;
  var match = this._input.match(this._re);
  if (!match) throw new Gin.Error(Gin.Error.INVALID_TOKEN);
  this._input = this._input.substring(match[0].length);
  var token = new String(match[0]);
  token.type = match[1] ? "ident" :
               match[2] ? "repeat" :
               match[3] ? "string" :
               match[4] ? "regexp" :
               match[5] ? "predef" : match[0];
  this._next = token;
  return token;
};
Gin.Rule.Tokenizer.prototype._re =
  /^(?:(\w+)|[()|-]|::?|([*+?]|\{\d+(?:,\d*)?\})|(\[(?:[^\\\]]|\\.)*\]|'(?:[^\\']|\\.)*'|"(?:[^\\"]|\\.)*")|(<(?:[^\\[>]|\\.|\[(?:[^\\\]]|\\.)*\])*>\w*)|(\$\w+))/;


// ==== Grammar ====

Gin.Grammar = function (rules, start, skip) {
  if (!(start in rules))
    throw new Gin.Error(Gin.Error.UNDEFINED_SYMBOL);
  rules = Gin.Utils.extend({}, rules);
  for (var i in rules)
    rules[i] = new Gin.Rule(rules[i], null, rules, i);
  this.rules = rules;
  this._start = start;
  this._skip = skip;
};
Gin.Utils.inherit(Gin.Grammar, Gin.Parser);
Gin.Grammar.prototype.parse =
function (scanner, handler) {
  scanner = Gin.Scanner(scanner, this._skip);
  var start = scanner.getIndex();
  var match = this.rules[this._start].parse([scanner, this._skip], handler);
  if (!match) {
    scanner.setIndex(start);
    return null;
  }
  scanner.moveIndex(match.lastIndex);
  return new Gin.Match(match.value, scanner);
};


// ==== Match ====

Gin.Match = function (value, scanner) {
  this.value = value;
  this.full = scanner.atEnd();
  this.lastIndex = scanner.getIndex();
};


// ==== Scanner ====

Gin.Scanner = function (input, skip) {
  var Scanner = arguments.callee;
  if (!(this instanceof Scanner))
    return (input instanceof Scanner) ? input : new Scanner(input, skip);
  if (input instanceof Scanner) {
    skip = skip || input._skip;
  } else if (input instanceof Array) {
    skip = input[1];
    input = input[0];
  }
  this._input = String(input);
  this._index = 0;
  this._skip =
    (skip instanceof Gin.Parser) ? skip :
    (skip instanceof RegExp) ? new Gin.Parser.RegExp(skip) :
    skip ? new Gin.Parser.String(skip) : Gin.SPACE;
  return this;
};
Gin.Utils.inherit(Gin.Scanner, String);
Gin.Scanner.prototype.getIndex =
function () {
  return this._index;
};
Gin.Scanner.prototype.setIndex =
function (index) {
  this._index = index;
};
Gin.Scanner.prototype.moveIndex =
function (offset) {
  this._index += offset;
};
Gin.Scanner.prototype.skip =
function () {
  if (this._skip == Gin.NOTHING) return;
  var input = this._input.substring(this._index);
  var scanner = new Gin.Scanner(input, Gin.NOTHING);
  while (this._skip.parse(scanner))
    ;
  this.moveIndex(scanner.getIndex());
};
Gin.Scanner.prototype.atEnd =
function () {
  this.skip();
  return this._index >= this._input.length;
};
Gin.Scanner.prototype.toString =
function () {
  this.skip();
  return this._input.substring(this._index);
};
Gin.Scanner.prototype.valueOf = Object.prototype.valueOf;


// ==== Error ====

Gin.Error = function (message) {
  this.message = String(message || "");
};
Gin.Utils.inherit(Gin.Error, Error);
Gin.Error.prototype.name = "GinError";

Gin.Error.NOT_IMPLEMENTED = "not implemented";
Gin.Error.INVALID_TOKEN = "invalid token";
Gin.Error.INVALID_RULE = "invalid rule";
Gin.Error.UNDEFINED_SYMBOL = "undefined symbol";
Gin.Error.NO_MATCH = "no match";


// ==== Predefined Parsers ====

Gin.ANY = new Gin.Parser.RegExp(/[\S\s]/);
Gin.SPACE = new Gin.Parser.RegExp(/[ \t\r\n]/);
Gin.BLANK = new Gin.Parser.RegExp(/[ \t]/);
Gin.EOL = new Gin.Parser.RegExp(/\n|\r\n?/);
Gin.NL = new Gin.Parser.RegExp(/[\n\u2028\u2029]|\r\n?/);
Gin.NOTHING = new Gin.Parser.Nothing();
Gin.END = new Gin.Parser.End();
Gin.EPSILON = new Gin.Parser.String("");
Gin.EPS = Gin.EPSILON;

Gin.UINT = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/[1-9][0-9]*|0/), Number
);
Gin.INT = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/[+-]?(?:[1-9][0-9]*|0)/), Number
);
Gin.UDECIMAL = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/(?:[1-9][0-9]*|0)(?:\.[0-9]*)?|\.[0-9]+/), Number
);
Gin.DECIMAL = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/[+-]?(?:(?:[1-9][0-9]*|0)(?:\.[0-9]*)?|\.[0-9]+)/),
  Number
);
Gin.UREAL = new Gin.Parser.Action(
  new Gin.Parser.RegExp(
    /(?:(?:[1-9][0-9]*|0)(?:\.[0-9]*)?|\.[0-9]+)(?:[Ee][+-]?[0-9]+)?/
  ),
  Number
);
Gin.REAL = new Gin.Parser.Action(
  new Gin.Parser.RegExp(
    /[+-]?(?:(?:[1-9][0-9]*|0)(?:\.[0-9]*)?|\.[0-9]+)(?:[Ee][+-]?[0-9]+)?/
  ),
  Number
);
Gin.OCTAL = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/0[0-7]+/),
  function (value) { return parseInt(value, 8); }
);
Gin.HEX = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/0[Xx][0-9A-Fa-f]+/), Number
);
Gin.JS_NUMBER = new Gin.Parser.Action(
  new Gin.Parser.RegExp(
    /(?:0(?:([0-7]+)|[Xx][0-9A-Fa-f]+)|(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[Ee][+-]?[0-9]+)?)(?![\w$])/
  ),
  function (value) { return value.match[1] ? parseInt(value, 8) : +value; }
);

Gin.CSV_STRING = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/"(?:[^"]|"")*"/),
  function (value) { return value.slice(1, -1).replace(/""/g, '"'); }
);
Gin.JS_STRING = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/"(?:[^\\"]|\\.)*"|'(?:[^\\']|\\.)*'/),
  Gin.Utils.unquote
);
Gin.STRING = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/(?:[^\(\)\|\*\+\{\}\?<>])+/),
  String
);
Gin.SYMBOL = new Gin.Parser.Action(
  new Gin.Parser.RegExp(/[a-zA-Z0-9_-]+/),
  String
);

// for node.js
for (var x in Gin)
	exports[x] = Gin[x];

