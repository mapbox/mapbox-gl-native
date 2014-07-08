#!/usr/bin/env node
'use strict';

var fuzzer = require('fuzzer');
var path = require('path');
var fs = require('fs');
var mkdirp = require('./mkdirp');
var json = require(path.join(process.cwd(), process.argv[2]));

fuzzer.seed(0);

json.constants = Object.keys(json.constants).reduce(function(obj, key, index) {
  var value = json.constants[key];
  if (typeof value === 'string') {
    obj[key] = fuzzer.mutate.string(value);
  }
  return obj;
}, {});

var data = JSON.stringify(json);

var out_path = path.join(process.argv[3], 'bin/style-fuzzed.min.js');
mkdirp.sync(path.dirname(out_path));
fs.writeFileSync(out_path, data);
