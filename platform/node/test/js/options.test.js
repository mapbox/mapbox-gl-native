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

function makeMapPool(size) {
    var mapPool = [];

    for (var i = 0; i < size; ++i) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        mapPool.push(map);
    }

    return mapPool;
};

test('Usage', function(t) {
    t.test('rendering after exception', function(t) {
        var map = new mbgl.Map(options);
        map.load(mockfs.style_vector);

        map.render({ debug: { tileBorders: "abc" } }, function(err, pixels) {
            if (err) {
                map.render({}, function(err, pixels) { if (!err) t.end(); });
            }
        });
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

test('Center', function(t) {
    t.test('sanity', function(t) {
        var map = new mbgl.Map(options);

        t.plan(1);

        map.load(mockfs.style_vector);
        map.render({ center: [10, 10] }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var mapPool = makeMapPool(10);
        var callback = function(err, pixels) { if (err) t.pass(); };

        t.plan(10);

        mapPool[0].render({ center: 0 }, callback);;
        mapPool[1].render({ center: "abc" }, callback);
        mapPool[2].render({ center: {} }, callback);
        mapPool[3].render({ center: [0, 0, 0] }, callback);
        mapPool[4].render({ center: ["abc", "def"] }, callback);
        mapPool[5].render({ center: ["123", "123"] }, callback);
        mapPool[6].render({ center: [123, "123"] }, callback);
        mapPool[7].render({ center: ["123", 123] }, callback);
        mapPool[8].render({ center: [-1000, -1000] }, callback);
        mapPool[9].render({ center: [1000, 1000] }, callback);
    });
})

test('Zoom', function(t) {
    t.test('sanity', function(t) {
        var mapPool = makeMapPool(3);

        t.plan(3);

        mapPool[0].render({ zoom: 10 }, function() { t.pass(); });
        mapPool[1].render({ zoom: -10 }, function() { t.pass(); });
        mapPool[2].render({ zoom: 100 }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var mapPool = makeMapPool(3);
        var callback = function(err, pixels) { if (err) t.pass(); };

        t.plan(3);

        mapPool[0].render({ zoom: "abc" }, callback);
        mapPool[1].render({ zoom: {} }, callback);
        mapPool[2].render({ zoom: [0, 0, 0] }, callback);
    });
});

test('Bearing', function(t) {
    t.test('sanity', function(t) {
        var mapPool = makeMapPool(3);

        t.plan(3);

        mapPool[0].render({ bearing: 90 }, function() { t.pass(); });
        mapPool[1].render({ bearing: -1000 }, function() { t.pass(); });
        mapPool[2].render({ bearing: 1000 }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var mapPool = makeMapPool(3);
        var callback = function(err, pixels) { if (err) t.pass(); };

        t.plan(3);

        mapPool[0].render({ bearing: "abc" }, callback);
        mapPool[1].render({ bearing: {} }, callback);
        mapPool[2].render({ bearing: [0, 0, 0] }, callback);
    });
});

test('Pitch', function(t) {
    t.test('sanity', function(t) {
        var mapPool = makeMapPool(3);

        t.plan(3);

        mapPool[0].render({ pitch: 10 }, function() { t.pass(); });
        mapPool[1].render({ pitch: -1000 }, function() { t.pass(); });
        mapPool[2].render({ pitch: 1000 }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var mapPool = makeMapPool(3);
        var callback = function(err, pixels) { if (err) t.pass(); };

        t.plan(3);

        mapPool[0].render({ pitch: "abc" }, callback);
        mapPool[1].render({ pitch: {} }, callback);
        mapPool[2].render({ pitch: [0, 0, 0] }, callback);
    });
});

test('Classes', function(t) {
    t.test('sanity', function(t) {
        var mapPool = makeMapPool(2);

        t.plan(2);

        mapPool[0].render({ classes: [] }, function() { t.pass(); });
        mapPool[1].render({ classes: ["abc"] }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var mapPool = makeMapPool(3);
        var callback = function(err, pixels) { if (err) t.pass(); };

        t.plan(3);

        mapPool[0].render({ classes: "abc" }, callback);
        mapPool[1].render({ classes: ["abc", 0] }, callback);
        mapPool[2].render({ classes: [0, 0, 0] }, callback);
    });
});

test('Debug', function(t) {
    t.test('sanity', function(t) {
        var mapPool = makeMapPool(6);

        t.plan(6);

        mapPool[0].render({ debug: [] }, function() { t.pass(); });
        mapPool[1].render({ debug: { tileBorders: true } }, function() { t.pass(); });
        mapPool[2].render({ debug: { parseStatus: true } }, function() { t.pass(); });
        mapPool[3].render({ debug: { timestamps: true } }, function() { t.pass(); });
        mapPool[4].render({ debug: { collision: true } }, function() { t.pass(); });
        mapPool[5].render({ debug: { overdraw: true } }, function() { t.pass(); });
    });

    t.test('invalid options', function(t) {
        var mapPool = makeMapPool(5);
        var callback = function(err, pixels) { if (err) t.pass(); };

        t.plan(5);

        mapPool[0].render({ debug: { tileBorders: "abc" } }, callback);
        mapPool[1].render({ debug: { parseStatus: "abc" } }, callback);
        mapPool[2].render({ debug: { timestamps: "abc" } }, callback);
        mapPool[3].render({ debug: { collision: "abc" } }, callback);
        mapPool[4].render({ debug: { overdraw: "abc" } }, callback);
    });
});
