var fs           = require('fs')
  , path         = require('path')
  , EventEmitter = require('events').EventEmitter
  , Grammar      = require('./grammar')
  , Julius
;

if ( fs.existsSync( path.join(__dirname, './build/Debug') ) ) {
	Julius = require('./build/Debug/julius').Julius;
} else if ( fs.existsSync( path.join(__dirname, './build/Release') ) ) {
	Julius = require('./build/Release/julius').Julius;
} else {
	throw '"Julius" has not been compiled yet.'
}

Julius.prototype.__proto__ = EventEmitter.prototype;
Julius.Grammar = Grammar;

module.exports = Julius;

