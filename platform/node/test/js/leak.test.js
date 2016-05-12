'use strict';

var test = require('tape');
var mbgl = require('../../../../lib/mapbox-gl-native');
var fs = require('fs');
var path = require('path');
var style = require('../fixtures/style.json');

var options = {
    request: function(req, callback) {
        fs.readFile(path.join(__dirname, '..', req.url), function(err, data) {
            callback(err, { data: data });
        });
    }
};

test('Leak', function(t) {
    t.test('returns an image', function(t) {
        var map = new mbgl.Map(options);
        map.load(style);
        map.render({}, function(err, pixels) {
            t.error(err);
            map.release();
            t.ok(pixels);
            t.ok(pixels instanceof Buffer);
            t.equal(pixels.length, 512 * 512 * 4)
            t.end();
        });
    });
});
