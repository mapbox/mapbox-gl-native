#!/usr/bin/env node
'use strict';

var path = require('path');
var fs = require('fs');
var mkdirp = require('./mkdirp');
var data = JSON.stringify(require(path.join(process.cwd(), process.argv[2])));

var out_path = path.join(process.argv[3], 'bin/style.min.js');
mkdirp.sync(path.dirname(out_path));
fs.writeFileSync(out_path, data);
