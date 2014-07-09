#!/usr/bin/env node
'use strict';

var load = require('./load-style.js');
var JSONStream = require('JSONStream');
var stringify = require('./stringify');
var mkdirp = require('./mkdirp.js');

load(process.argv[2])
  .pipe(stringify())
  .pipe(mkdirp(process.argv[3]));
