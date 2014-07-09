#!/usr/bin/env node
'use strict';

var concat = require('concat-stream');
var fuzzer = require('fuzzer');
fuzzer.seed(0);

var read = process.stdin;
var write = concat(function(buffer) {
  var json = JSON.parse(buffer);

  json.constants = Object.keys(json.constants).reduce(function(obj, key, index) {
    var value = json.constants[key];
    if (typeof value === 'string') {
      obj[key] = fuzzer.mutate.string(value);
    }
    return obj;
  }, {});

  var data = JSON.stringify(json);
  process.stdout.write(data);
});

read.pipe(write);
