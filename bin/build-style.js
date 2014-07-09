#!/usr/bin/env node
'use strict';

var load = require('./load-style.js');
var mkdirp = require('./mkdirp.js');
var path = require('path');

var transforms = {
  'style.min.js': require('./minify.js'),
  'fuzz-colors.min.js': require('./fuzz-colors.js')
};

var source = load(process.argv[2]);

Object.keys(transforms).forEach(function(key) {
  load(process.argv[2])
    .pipe(transforms[key].call())
    .pipe(mkdirp(path.join(process.argv[3], key)));
});
