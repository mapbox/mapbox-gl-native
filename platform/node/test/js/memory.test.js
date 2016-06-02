'use strict';

var test = require('tape');
var mbgl = require('../../../../lib/mapbox-gl-native');
var request = require('request');
var fs = require('fs');
var path = require('path');
var style = require('../fixtures/tilejson/local.json');
var prettyBytes = require('pretty-bytes');
var queue = require('d3-queue').queue;
var zlib = require('zlib');

test('Memory', function(t) {
    var options = {
        request: function(req, callback) {
            request(req.url, function(err, res) {
                if (err) return callback(err);
                callback(null, { data: new Buffer(res.body) });
            });
        }
    };

    t.skip('setup', function(t) {
        t.equal(JSON.stringify(style).length, 320);
        t.end();
    });

    t.skip('bench Map.release', function(t) {
        // Garbage collection before initial benchmark
        if (typeof gc === 'function') gc();

        var init = process.memoryUsage().heapUsed;
        var maps = [];

        for (var i = 0; i < 5e4; i++) {
            (function() {
                var map = new mbgl.Map(options);
                map.release();
            })();
        }

        // t.equal(maps.length, 256, 'all maps added to array');

        if (typeof gc === 'function') { gc() }
        console.log(process.memoryUsage());

        t.equal((process.memoryUsage().heapUsed - init) < 15e6, true, 'map.release heapUsed ' + prettyBytes(process.memoryUsage().heapUsed - init));

        t.end();
    });

    t.skip('bench Map.load', function(t) {
        var map = new mbgl.Map(options);
        
        var time = +new Date;
        var mem = process.memoryUsage().rss;

        for (var i = 0; i < 256; i++) { map.load(style); }

        time = (+new Date - time);
        mem = process.memoryUsage().rss - mem;

        // TODO: This time makes me think Map.load might be async.
        t.equal(time < 10, true, 'load x256 took ' + time + 'ms');
        t.equal(mem < 15e5, true, 'load rss + ' + mem + ' bytes');

        map.release();

        t.end();
    });

    t.skip('bench Map.render', function(t) {
        var iterations = 10e3;
        var map = new mbgl.Map(options);
        map.load(require('../fixtures/satellite-v9.json'));
        
        var time = +new Date;
        var mem = process.memoryUsage().rss;

        var q = queue(1);

        var deferredRender = function(callback) {
            map.render({}, function(err, pixels) {
                callback(err);
            });
        };

        for (var i = 0; i < iterations; i++) {
            q.defer(deferredRender);
        }

        q.awaitAll(function(err) {
            t.error(err);

            time = (+new Date - time);
            mem = process.memoryUsage().rss - mem;
            t.equal(time < 80e3, true, 'render x' + iterations + ' took ' + time + 'ms');
            t.equal(mem < 2e9, true, 'render rss + ' + prettyBytes(mem));
            t.end();
        });
    });

    t.end();
});


/*
(function() {
var cache = new Cache('a', 256);

[1,2,3,4].forEach(function(j) {
    tape('bench loadSync (LRU) x' + j, function(assert) {
        var mem = process.memoryUsage().rss;
        for (var i = 0 * j; i < 1024 * j; i++) cache.loadSync(data, 'stat', i);
        setTimeout(function() {
            mem = process.memoryUsage().rss - mem;
            if (j === 1) {
                assert.equal(mem < 2e9, true, 'loadSync rss + ' + mem + ' bytes');
            } else {
                assert.equal(mem <= 5e6, true, 'loadSync rss + ' + mem + ' bytes');
            }
            assert.end();
        }, 2000);
    });
});

})();
*/

