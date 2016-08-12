'use strict';

var test = require('tape');
var mbgl = require('../../../../lib/mapbox-gl-native');
var fs = require('fs');
var path = require('path');
var style = require('../fixtures/style.json');
var glyphs = require('../fixtures/glyphs.json');
var sprite = require('../fixtures/sprite.json');

mbgl.on('message', function(msg) {
    console.log('%s (%s): %s', msg.severity, msg.class, msg.text);
});

test('Slow tiles', function(t) {
    var options = {
        request: function(req, callback) {
            function readFile() {
                fs.readFile(path.join(__dirname, '..', req.url), function(err, data) {
                    callback(err, { data: data });
                });
            }

            switch (req.kind) {
                case 3:
                    setTimeout(readFile, 1000);
                    break;
                default:
                   readFile(); 
            }
        },
        ratio: 1
    };

    var map = new mbgl.Map(options);
    map.load(style);

    map.render({}, function(err, pixels) {
        t.error(err);
        t.end();
    });
});

test('Partial tiles with slow resources', function(t) {
    var options = {
        request: function(req, callback) {
            function readFile() {
                fs.readFile(path.join(__dirname, '..', req.url), function(err, data) {
                    callback(err, { data: data });
                });
            }

            switch (req.kind) {
                case 4:
                case 5:
                case 6:
                    setTimeout(readFile, 1000);
                    break;
                default:
                   readFile(); 
            }
        },
        ratio: 1
    };

    t.test('Slow glyphs', function(t) {
        var map = new mbgl.Map(options);
        map.load(glyphs);

        map.render(glyphs, function(err, pixels) {
            t.error(err);
            t.end();
        });
    });

    t.test('Slow sprite', function(t) {
        var map = new mbgl.Map(options);
        map.load(sprite);

        map.render(sprite, function(err, pixels) {
            t.error(err);
            t.end();
        });
    });

    t.end();
});
