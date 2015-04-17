#!/usr/bin/env node
/* jshint node: true, unused: false */
'use strict';

var mbgl = require('..');
var fs = require('fs');
var path = require('path');

var base = path.join(path.dirname(process.mainModule.filename), '../test');

var fileSource = new mbgl.FileSource();
fileSource.request = function(req) {
    fs.readFile(path.join(base, req.url), function(err, data) {
        req.respond(err, { data: data });
    });
};

fileSource.cancel = function(req) {
};

var map = new mbgl.Map(fileSource);
map.load(require('../test/fixtures/style.json'));
map.render({}, function(err, image) {
    if (err) throw err;
    mbgl.compressPNG(image, function(err, png) {
        fs.writeFileSync('image.png', png);
        console.warn('Written image.png');
    });
});
