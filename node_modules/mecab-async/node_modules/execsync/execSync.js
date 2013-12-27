var fs   = require('fs')
  , path = require('path')
  , execSync
;

if ( fs.existsSync( path.join(__dirname, './build/Debug') ) ) {
	execSync = require('./build/Debug/shell').execSync;
} else if ( fs.existsSync( path.join(__dirname, './build/Release') ) ) {
	execSync = require('./build/Release/shell').execSync;
} else {
	throw '"execsync" has not been compiled yet.'
}

module.exports = execSync;
