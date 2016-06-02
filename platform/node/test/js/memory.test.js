'use strict';

var test = require('tape');
var mbgl = require('../../../../lib/mapbox-gl-native');
var fs = require('fs');
var path = require('path');
var style = require('../fixtures/style.json');

test('Memory', function(t) {
    var options = {
        request: function(req, callback) {
            fs.readFile(path.join(__dirname, '..', req.url), function(err, data) {
                callback(err, { data: data });
            });
        }
    };

    t.test('setup', function(t) {
        t.equal(JSON.stringify(style).length, 320);
        t.end();
    });

    t.test('Map.release', function(t) {
        var mem = process.memoryUsage().rss;

        var map = new mbgl.Map(options);
        var create = process.memoryUsage().rss - mem;
        t.equal(create < 15e5, true, 'load rss + ' + create + ' bytes');

        var prerelease = process.memoryUsage().rss;
        map.release();
        if (typeof gc === 'function') gc();
        var postrelease = process.memoryUsage().rss;

        // TODO: This should be a positive number, as memory usage should
        // shrink after calling Map.release() and a garbage collection pass
        // occurs, right?
        var destroy = prerelease - postrelease;
        t.equal(destroy > 0, true, 'release rss ' + destroy + ' bytes');

        t.end();
    });

    t.test('bench Map.load', function(t) {
        var map = new mbgl.Map(options);
        
        var time = +new Date;
        var mem = process.memoryUsage().rss;

        for (var i = 0; i < 256; i++) { map.load(style); }

        time = (+new Date - time);
        mem = process.memoryUsage().rss - mem;

        // TODO: This time makes me think Map.load might be async.
        t.equal(time < 5, true, 'load x256 took ' + time + 'ms');
        t.equal(mem < 15e5, true, 'load rss + ' + mem + ' bytes');

        map.release();

        t.end();
    });

    t.skip('bench Map.render', function(t) {
        var map = new mbgl.Map(options);
        map.load(style);
        
        var time = +new Date;
        var mem = process.memoryUsage().rss;

        /*
        for (var i = 0; i < 256; i++) map.load(style);
        time = (+new Date - time);
        mem = process.memoryUsage().rss - mem;
        t.equal(time < 80e3, true, 'loadSync x256 took ' + time + 'ms');
        t.equal(mem < 2e9, true, 'loadSync rss + ' + mem + ' bytes');
        t.end();
        */

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

