'use strict';

var fs = require('fs');
var mbgl = require('../index');
var path = require('path');
var test = require('tape');

var testParams = {
    mapPoolSize: 10,
    numRenderings: 100,
    heapGrowthThreshold: 0,
    ratio: 2
};

function readFixture(file) {
    return fs.readFileSync(path.join('test/fixtures/resources', file));
}

var style_raster  = readFixture('style_raster.json').toString('utf8');
var style_vector  = readFixture('style_vector.json').toString('utf8');
var sprite_json   = readFixture('sprite.json');
var sprite_png    = readFixture('sprite.png');
var glyph         = readFixture('glyphs.pbf');
var source_raster = readFixture('source_raster.json');
var source_vector = readFixture('source_vector.json');
var tile_raster   = readFixture('raster.tile');
var tile_vector   = readFixture('vector.tile');

test('Memory', function(t) {
    // Trigger garbage collection before starting test, then initialize 
    // heap size
    if (typeof gc === 'function') gc();
    var lastHeapSize = process.memoryUsage()['heapUsed'];

    var options = {
        request: function(req, callback) {
            if (req.url == null) {
                t.fail('invalid file request');
            } else if (req.url.indexOf('sprite') > -1 && req.url.endsWith('json')) {
                callback(null, { data: sprite_json });
            } else if (req.url.indexOf('sprite') > -1 && req.url.endsWith('png')) {
                callback(null, { data: sprite_png });
            } else if (req.url.indexOf('fonts') > -1 && req.url.endsWith('pbf')) {
                callback(null, { data: glyph });
            } else if (req.url.endsWith('mapbox.satellite')) {
                callback(null, { data: source_raster });
            } else if (req.url.indexOf('satellite') > -1 && (req.url.endsWith('png') || req.url.endsWith('webp'))) {
                callback(null, { data: tile_raster });
            } else if (req.url.endsWith('mapbox.mapbox-streets-v7')) {
                callback(null, { data: source_vector });
            } else if (req.url.indexOf('streets') > -1 && req.url.endsWith('pbf')) {
                callback(null, { data: tile_vector });
            } else {
                t.fail('unhandled file request: ' + req.url);
            }
        },
        ratio: testParams.ratio,
    };

    var mapPool = []

    for (var i = 0; i < testParams.mapPoolSize; ++i) {
        var map = new mbgl.Map(options);
        mapPool.push(map);
    }

    var renderCount = 0;
    var heapGrowth = 0;

    var interval = setInterval(function () {
        if (mapPool.length == 0) {
            return;
        }

        var map = mapPool.shift();

        if (Math.floor(Math.random() * 2)) {
            map.load(style_raster);
        } else {
            map.load(style_vector);
        }

        map.render({ zoom: 16 }, function(err, pixels) {
            mapPool.push(map);

            if (renderCount % (testParams.numRenderings / 10) == 0) {
                // Manually trigger garbage collection
                if (typeof gc === 'function') gc();
                
                var currentHeapSize = process.memoryUsage()['heapUsed'];

                // Print some progress, so slow build bots don't timeout.
                t.comment("Rendering (" + renderCount.toString() +
                    "/" + testParams.numRenderings.toString() + ")");

                heapGrowth = heapGrowth + currentHeapSize - lastHeapSize;
                lastHeapSize = currentHeapSize;
            }

            if (++renderCount == testParams.numRenderings) {
                t.ok(heapGrowth < testParams.heapGrowthThreshold, heapGrowth);
                t.end();

                clearInterval(interval);
            }
        });
    }, 1);
});
