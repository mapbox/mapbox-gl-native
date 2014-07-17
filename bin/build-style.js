#!/usr/bin/env node
'use strict';

var mkdirp = require('mkdirp');
var path = require('path');
var fs = require('fs');

var data = JSON.stringify(require(path.join(process.cwd(), process.argv[2])));
var out = path.join(process.argv[3], 'style.min.js');

mkdirp.sync(path.dirname(out));
fs.writeFileSync(out, data);
