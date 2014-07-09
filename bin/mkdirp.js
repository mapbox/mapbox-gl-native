#!/usr/bin/env node
'use strict';

var path = require('path');
var mkdirp = require('mkdirp');
var fs = require('fs');

module.exports = function(out) {
  mkdirp.sync(path.dirname(out));
  return fs.createWriteStream(out);
};

if (!module.parent) process.stdin.pipe(module.exports(process.argv[2]));
