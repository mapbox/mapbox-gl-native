'use strict';

var test = require('tape');
var mbgl = require('../../../../lib/mapbox-gl-native');
var request = require('request');
var queue = require('d3-queue').queue;
var generator = require('generate-geo-testing-data')(
    { mode: 'latlon' },
    function(width, height, lat, lon, zoom) {
        return {
            center: [lat, lon],
            zoom: zoom
        };
    }
);
var prettyBytes = require('pretty-bytes');

test('Render', function(t) {
    t.test('MapboxAccessToken', function(t) {
        t.notEqual(process.env.MapboxAccessToken, undefined, 'MapboxAccessToken environment variable is defined');
        t.end();
    });

    function renderBench(t, style) {
        var map = new mbgl.Map({
            request: function(req, callback) {
                request({
                  url: req.url,
                  qs: { 'access_token': process.env.MapboxAccessToken },
                  gzip: true,
                  encoding: null
                }, function(err, res) {
                    if (err) return callback(err);
                    callback(null, { data: res.body });
                });
            }
        });

        map.load(style);
        
        var q = queue(1);
        var iterations = 10e3;

        var time = +new Date;
        // var mem = process.memoryUsage().rss;

        var deferredRender = function(callback) {
            generator(function(options) {
                map.render(options, function(err, pixels) {
                    console.log(prettyBytes(process.memoryUsage().rss));
                    callback(err);
                });
            });
        };

        for (var i = 0; i < iterations; i++) {
            q.defer(deferredRender);
        }

        q.awaitAll(function(err) {
            t.error(err);

            time = (+new Date - time);
            // mem = process.memoryUsage().rss - mem;
            t.equal(time > 0, true, 'render x' + iterations + ' took ' + time + 'ms');
            // t.equal(mem > 0, true, 'render rss ' + prettyBytes(mem));

            t.end();
        });
    }

    t.test('bench Map.render vector', function(t) {
        renderBench(t, require('../fixtures/tilejson/vector.json'));
    });

    t.test('bench Map.render raster', function(t) {
        renderBench(t, require('../fixtures/tilejson/raster.json'));
    });
});
