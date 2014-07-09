#!/usr/bin/env node
'use strict';

var load = require('./load-style.js');
var minify = require('./minify');
var mkdirp = require('./mkdirp.js');

load(process.argv[2])
  .pipe(minify())
  .pipe(mkdirp(process.argv[3]));
