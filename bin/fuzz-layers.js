#!/usr/bin/env node
'use strict';

var through = require('through2');
var fuzzer = require('fuzzer');
fuzzer.seed(0);

module.exports = function() {
  return through.obj(function(chunk, env, callback) {
    var json = JSON.parse(chunk.toString());

    json.layers.map(function(value) {
      var mutator = fuzzer.mutate.object(value);
      return mutator();
    });

    var data = JSON.stringify(json);
    this.push(data);
    callback();
  });
};

if (!module.parent) process.stdin.pipe(module.exports()).pipe(process.stdout);
