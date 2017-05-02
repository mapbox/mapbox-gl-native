'use strict';

var PNG = require('pngjs').PNG;
var fs = require('fs');
var mbgl = require('../../index');
var mockfs = require('./../mockfs');
var path = require('path');
var pixelmatch = require('pixelmatch');
var test = require('tape');

var options = {
    request: function(req, callback) {
        callback(null, { data: mockfs.dataForRequest(req) });
    },
    ratio: 1,
};

test('Center', function(t) {
    t.test('sanity', function(t) {
        var map = new mbgl.Map(options);

        t.plan(1);

        map.load(mockfs.style_vector);
        map.render({ center: [10, 10] }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        // Should never render, it will throw
        var callback = function(err, pixels) { t.fail(); };

        t.throws(function() { map.render({ center: 0 }, callback); });
        t.throws(function() { map.render({ center: "abc" }, callback); });
        t.throws(function() { map.render({ center: {} }, callback); });
        t.throws(function() { map.render({ center: [0, 0, 0] }, callback); });
        t.throws(function() { map.render({ center: ["abc", "def"] }, callback); });
        t.throws(function() { map.render({ center: ["123", "123"] }, callback); });
        t.throws(function() { map.render({ center: [123, "123"] }, callback); });
        t.throws(function() { map.render({ center: ["123", 123] }, callback); });
        t.throws(function() { map.render({ center: [-1000, -1000] }, callback); });
        t.throws(function() { map.render({ center: [1000, 1000] }, callback); });

        t.end();
    });
})

test('Zoom', function(t) {
    t.test('sanity', function(t) {
        var map1 = new mbgl.Map(options);
        var map2 = new mbgl.Map(options);
        var map3 = new mbgl.Map(options);

        map1.load(mockfs.style_vector);
        map2.load(mockfs.style_vector);
        map3.load(mockfs.style_vector);

        t.plan(3);

        // Should clamp and not throw
        map1.render({ zoom: 10 }, function() { t.pass(); });
        map2.render({ zoom: -10 }, function() { t.pass(); });
        map3.render({ zoom: 100 }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        // Should never render, it will throw
        var callback = function(err, pixels) { t.fail(); };

        t.throws(function() { map.render({ zoom: "abc" }, callback); });
        t.throws(function() { map.render({ zoom: {} }, callback); });
        t.throws(function() { map.render({ zoom: [0, 0, 0] }, callback); });

        t.end();
    });
});

test('Bearing', function(t) {
    t.test('sanity', function(t) {
        var map1 = new mbgl.Map(options);
        var map2 = new mbgl.Map(options);
        var map3 = new mbgl.Map(options);

        map1.load(mockfs.style_vector);
        map2.load(mockfs.style_vector);
        map3.load(mockfs.style_vector);

        t.plan(3);

        // Should wrap and not throw
        map1.render({ bearing: 90 }, function() { t.pass(); });
        map2.render({ bearing: -1000 }, function() { t.pass(); });
        map3.render({ bearing: 1000 }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        // Should never render, it will throw
        var callback = function(err, pixels) { t.fail(); };

        t.throws(function() { map.render({ bearing: "abc" }, callback); });
        t.throws(function() { map.render({ bearing: {} }, callback); });
        t.throws(function() { map.render({ bearing: [0, 0, 0] }, callback); });

        t.end();
    });
});

test('Pitch', function(t) {
    t.test('sanity', function(t) {
        var map1 = new mbgl.Map(options);
        var map2 = new mbgl.Map(options);
        var map3 = new mbgl.Map(options);

        map1.load(mockfs.style_vector);
        map2.load(mockfs.style_vector);
        map3.load(mockfs.style_vector);

        t.plan(3);

        // Should clamp and not throw
        map1.render({ pitch: 10 }, function() { t.pass(); });
        map2.render({ pitch: -1000 }, function() { t.pass(); });
        map3.render({ pitch: 1000 }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        // Should never render, it will throw
        var callback = function(err, pixels) { t.fail(); };

        t.throws(function() { map.render({ pitch: "abc" }, callback); });
        t.throws(function() { map.render({ pitch: {} }, callback); });
        t.throws(function() { map.render({ pitch: [0, 0, 0] }, callback); });

        t.end();
    });
});

test('Classes', function(t) {
    t.test('sanity', function(t) {
        var map1 = new mbgl.Map(options);
        var map2 = new mbgl.Map(options);

        map1.load(mockfs.style_vector);
        map2.load(mockfs.style_vector);

        t.plan(2);

        map1.render({ classes: [] }, function() { t.pass(); });
        map2.render({ classes: ["abc"] }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        // Should never render, it will throw
        var callback = function(err, pixels) { t.fail(); };

        t.throws(function() { map.render({ classes: "abc" }, callback); });
        t.throws(function() { map.render({ classes: ["abc", 0] }, callback); });
        t.throws(function() { map.render({ classes: [0, 0, 0] }, callback); });

        t.end();
    });
});

test('Debug', function(t) {
    t.test('sanity', function(t) {
        var map1 = new mbgl.Map(options);
        var map2 = new mbgl.Map(options);
        var map3 = new mbgl.Map(options);
        var map4 = new mbgl.Map(options);
        var map5 = new mbgl.Map(options);
        var map6 = new mbgl.Map(options);

        map1.load(mockfs.style_vector);
        map2.load(mockfs.style_vector);
        map3.load(mockfs.style_vector);
        map4.load(mockfs.style_vector);
        map5.load(mockfs.style_vector);
        map6.load(mockfs.style_vector);

        t.plan(6);

        map1.render({ debug: [] }, function() { t.pass(); });
        map2.render({ debug: { tileBorders: true } }, function() { t.pass(); });
        map3.render({ debug: { parseStatus: true } }, function() { t.pass(); });
        map4.render({ debug: { timestamps: true } }, function() { t.pass(); });
        map5.render({ debug: { collision: true } }, function() { t.pass(); });
        map6.render({ debug: { overdraw: true } }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        // Should never render, it will throw
        var callback = function(err, pixels) { t.fail(); };

        t.throws(function() { map.render({ debug: { tileBorders: "abc" } }, callback); });
        t.throws(function() { map.render({ debug: { parseStatus: "abc" } }, callback); });
        t.throws(function() { map.render({ debug: { timestamps: "abc" } }, callback); });
        t.throws(function() { map.render({ debug: { collision: "abc" } }, callback); });
        t.throws(function() { map.render({ debug: { overdraw: "abc" } }, callback); });

        t.end();
    });
});

test('Usage', function(t) {
    t.test('rendering after exception', function(t) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        // Should never render, it will throw
        var callback = function(err, pixels) { t.fail(); };

        t.throws(function() { map.render({ debug: { tileBorders: "abc" } }, callback); });
        map.render({}, function(err, pixels) { t.end(); });
    });

    t.test('default options', function(t) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        map.render({}, function(err, pixels) {
            var expected = PNG.sync.read(fs.readFileSync(path.join(__dirname, '../fixtures/options/expected.png'))).data;
            var numPixels = pixelmatch(pixels, expected, undefined, 512, 512, { threshold: 0.13 });
            t.equal(numPixels, 0);
            t.end();
        });
    });
});
