#!/usr/bin/env node
'use strict';

var load = require('./load-style.js');
var mkdirp = require('mkdirp');
var path = require('path');
var fs = require('fs');

var transforms = {
  'style.min.js': require('./minify.js'),
  'fuzz-colors.min.js': require('./fuzz-colors.js')
};

var source = load(process.argv[2]);
var out = process.argv[3];
mkdirp.sync(out);

Object.keys(transforms).forEach(function(key) {
  source.pipe(transforms[key].call())
    .pipe(fs.createWriteStream(path.join(out, key)));
});
