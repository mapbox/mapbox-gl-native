#!/usr/bin/env node
'use strict';

var concat = require('concat-stream');
var through = require('through2');
var fuzzer = require('fuzzer');
fuzzer.seed(0);

module.exports = function() {
  var read = through();
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
};

// if (!module.parent) module.exports().pipe(process.stdout);
