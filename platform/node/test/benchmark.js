'use strict';

var mockfs = require('./mockfs');
var mbgl = require('../index');
var test = require('tape');

var firstRequest = "mapbox://sprites/mapbox/streets-v9@2x.json";

var params = {
    mapPoolSize: 10,
    numRenderings: 1000,
    failurePercentage: 10,
    timeoutPercentage: 10,
    renderingTimeout: 5000,
    ratio: 2
};

test('Benchmark', function(t) {
    console.time('Time');

    var renderCount = 0;
    var failureCount = 0;
    var cancelCount = 0;

    var options = {
        request: function(req, callback) {
            setTimeout(function() {
                var num = Math.floor(Math.random() * 100);

                if (req.url == firstRequest && num < params.failurePercentage) {
                    callback(new Error('Failure'));
                } else if (req.url == firstRequest && num > 99 - params.timeoutPercentage) {
                    setTimeout(function() { callback(new Error('Timeout')); }, params.renderingTimeout * 5);
                } else {
                    var data = mockfs.dataForRequest(req);
                    callback(null, { data: mockfs.dataForRequest(req) });
                }
            }, 0);
        },
        ratio: params.ratio,
    };

    var mapPool = []

    for (var i = 0; i < params.mapPoolSize; ++i) {
        var map = new mbgl.Map(options);
        mapPool.push(map);
    }

    var interval = setInterval(function () {
        if (mapPool.length == 0 || renderCount == params.numRenderings) {
            return;
        }

        var map = mapPool.shift();

        map.load('{ "version": 8, "sources": {}, "layers": [] }');
        map.load(mockfs.style_vector);

        renderCount += 1;

        if (renderCount % (params.numRenderings / 100) == 0) {
            // Print some progress, so slow build bots don't timeout.
            t.comment('Rendering (' + renderCount.toString() +
                '/' + params.numRenderings.toString() + ')');
        }

        if (renderCount == params.numRenderings) {
            clearInterval(interval);
            t.end();
            console.timeEnd('Time');
            console.log('Failures: ' + failureCount);
            console.log('Canceled: ' + cancelCount);

            return;
        }

        var mapTimeout = setTimeout(function() {
            map.cancel();
        }, params.renderingTimeout);

        map.render({ zoom: 16 }, function(err, pixels) {
            clearTimeout(mapTimeout);

            if (err) {
                if (err.message == 'Failure') {
                    failureCount += 1;
                }

                if (err.message == 'Canceled') {
                    cancelCount += 1;
                }

                // We cancel the request before it gets a
                // timeout error from the file source.
                if (err.message == 'Timeout') {
                    t.fail('should never happen');
                }
            }

            mapPool.push(map);
        });
    }, 1);
});
