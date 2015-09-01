#!/usr/bin/env node
/* jshint node: true, unused: false */
'use strict';

var mbgl = require('../../..');
var fs = require('fs');
var path = require('path');
var PNG = require('pngjs').PNG;

var base = path.join(__dirname, '../test');

var map = new mbgl.Map({
    request: function(req) {
        fs.readFile(path.join(base, req.url), function(err, data) {
            req.respond(err, { data: data });
        });
    },
    ratio: 1.0
});

map.load(require('../test/fixtures/style.json'));

map.render({}, function(err, data) {
    if (err) throw err;

    var png = new PNG({
        width: data.width,
        height: data.height
    });

    png.data = data.pixels;

    png.pack()
        .pipe(fs.createWriteStream('image.png'))
        .on('finish', function() {
            console.warn('Written image.png');
        });
});
