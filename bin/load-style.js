#!/usr/bin/env node
'use strict';

var fs = require('fs');
var path = require('path');
var brfs = require('brfs');

module.exports = function(file) {
  var fromFile = file && file !== '-';
  var rs = fromFile ? fs.createReadStream(file) : process.stdin;
  var fpath = fromFile ? file : path.join(process.cwd(), '-');
  return rs.pipe(brfs(fpath));
};

if (!module.parent) module.exports(process.argv[2]).pipe(process.stdout);
