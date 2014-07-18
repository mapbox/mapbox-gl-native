#!/usr/bin/env node
'use strict';

var through = require('through2');

module.exports = function() {
  return through.obj(function(chunk, env, callback) {
    var json = JSON.parse(chunk.toString());
    var data = JSON.stringify(json);
    this.push(data);
    callback();
  });
};

if (!module.parent) process.stdin.pipe(module.exports()).pipe(process.stdout);
