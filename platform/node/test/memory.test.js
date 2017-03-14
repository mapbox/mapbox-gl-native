'use strict';

var mockfs = require('./mockfs');
var mbgl = require('../index');
var test = require('tape');

var testParams = {
    mapPoolSize: 10,
    numRenderings: 100,
    heapGrowthThreshold: 0,
    ratio: 2
};

test('Memory', function(t) {
    // Trigger garbage collection before starting test, then initialize
    // heap size
    if (typeof gc === 'function') gc();
    var lastHeapSize = process.memoryUsage()['heapUsed'];

    var options = {
        request: function(req, callback) {
            callback(null, { data: mockfs.dataForRequest(req) });
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
            map.load(mockfs.style_raster);
        } else {
            map.load(mockfs.style_vector);
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
