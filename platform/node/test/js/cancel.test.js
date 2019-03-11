'use strict';

var mockfs = require('./../mockfs');
var mbgl = require('../../index');
var test = require('tape');

var options = {
    request: function(req, callback) {
        callback(null, { data: mockfs.dataForRequest(req) });
    },
    ratio: 1,
};

test('Cancel', function(t) {
    t.test('sanity', function(t) {
        var renderCount = 0;
        var cancelCount = 0;
        var map = new mbgl.Map(options);

        var renderCallback = function(err, pixels) {
            if (err) {
                cancelCount++;
            } else {
                renderCount++;
            }
        };

        map.load(mockfs.style_vector);

        map.render({ zoom: 16 }, renderCallback);
        map.cancel();

        map.render({ zoom: 16 }, renderCallback);
        map.cancel();

        map.render({ zoom: 16 }, renderCallback);
        map.cancel();

        t.equal(renderCount, 0);
        t.equal(cancelCount, 3);

        t.end();
    });

    t.test('render after cancel', function(t) {
        var map = new mbgl.Map(options);
        var renderCallback = function(err, pixels) { if (!err) t.end(); };

        map.load(mockfs.style_vector);

        map.render({ zoom: 16 }, renderCallback);
        map.cancel();

        map.render({ zoom: 16 }, renderCallback);
    });

    t.test('cancel after cancel', function(t) {
        var cancelCount  = 0;
        var map = new mbgl.Map(options);

        map.load(mockfs.style_vector);
        map.render({ zoom: 16 }, function(err, pixels) {
            cancelCount++;
        });

        map.cancel();

        t.throws(function() {
            map.cancel();
        }, 'already canceled');

        t.equal(cancelCount, 1);

        t.end();
    });

    t.test('cancel without rendering', function(t) {
        var cancelCount  = 0;
        var map = new mbgl.Map(options);

        map.load(mockfs.style_vector);

        t.throws(function() {
            map.cancel();
        }, 'nothing to cancel');

        t.end();
    });

    t.test('release after cancel', function(t) {
        var map = new mbgl.Map(options);

        map.load(mockfs.style_vector);
        map.render({ zoom: 16 }, function(err, pixels) {});

        map.cancel();
        map.release();

        t.end();
    });

    t.test('cancel after release', function(t) {
        var map = new mbgl.Map(options);

        map.load(mockfs.style_vector);
        map.render({ zoom: 16 }, function(err, pixels) {});

        map.release();

        t.throws(function() {
            map.cancel();
        }, 'map resources already released');

        t.end();
    });
})
