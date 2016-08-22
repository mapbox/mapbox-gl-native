'use strict';

var test = require('tape').test;
var mbgl = require('../../../../lib/mapbox-gl-native');
var fs = require('fs');
var path = require('path');
var style = require('../fixtures/styles/style.json');
var glyphs = require('../fixtures/styles/glyphs.json');
var sprite = require('../fixtures/styles/sprite.json');
var satellite = require('../fixtures/styles/satellite-v8.json');
var streets = require('../fixtures/styles/streets.json');
var request = require('request');
var zlib = require('zlib');

mbgl.on('message', function(msg) {
    console.log('%s (%s): %s', msg.severity, msg.class, msg.text);
});

function readFile(req, callback) {
    if (/^http/.test(req.url)) {
        if (!process.env.MAPBOX_ACCESS_TOKEN) throw "Please set MAPBOX_ACCESS_TOKEN environment variable";

        request({
            url: req.url,
            qs: {
              'access_token': process.env.MAPBOX_ACCESS_TOKEN
            },
            gzip: true
        }, function(err, res) {
            if (err) return callback(err);

            // A non-buffer buffer can just be ignored
            if (res.body.length === undefined) return callback();

            callback(null, { data: new Buffer(res.body) });
        });
    } else {
        fs.readFile(path.join(__dirname, '..', req.url), function(err, data) {
            if (err) return callback(err);
            callback(err, { data: data });
        });
    }
}

test('Slow tiles', function(t) {
    var options = {
        request: function(req, callback) {
            switch (req.kind) {
                case 3:
                    setTimeout(function() {
                        readFile(req, callback);
                    }, 1000);
                    break;
                default:
                   readFile(req, callback); 
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
            switch (req.kind) {
                case 4:
                case 5:
                case 6:
                    setTimeout(function() {
                        readFile(req, callback);
                    }, 1000);
                    break;
                default:
                   readFile(req, callback); 
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

    t.test('Many slow resources', function(t) {
        var map = new mbgl.Map(options);
        map.load(streets);

        map.render({
            center: [-122.4403, 37.7497],
            zoom: 8,
            bearing: 20,
            width: 700,
            height: 700
        }, function(err, pixels) {
            t.error(err);
            t.end();
        });
    });

    t.end();
});
