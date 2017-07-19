'use strict';

var mockfs = require('../mockfs');
var mbgl = require('../../index');
var test = require('tape');

[ 'sprite_png', 'sprite_json', 'source_vector', 'glyph' ].forEach(function (resource) {
    test(`render reports an error when the request function responds with an error (${resource})`, function(t) {
        var map = new mbgl.Map({
            request: function(req, callback) {
                var data = mockfs.dataForRequest(req);
                if (mockfs[resource] === data) {
                    callback(new Error('message'));
                } else {
                    callback(null, { data: data });
                }
            }
        });
        map.load(mockfs.style_vector);
        map.render({ zoom: 16 }, function(err, pixels) {
            t.assert(err);
            t.assert(/message/.test(err.message));
            t.assert(!pixels);
            t.end();
        });
    });
});

[ 'vector', 'raster' ].forEach(function (type) {
    test(`render does not report an error when the request function responds with no data for a tile (${type})`, function(t) {
        var map = new mbgl.Map({
            request: function(req, callback) {
                var data = mockfs.dataForRequest(req);
                if (mockfs[`tile_${type}`] === data) {
                    callback();
                } else {
                    callback(null, { data: data });
                }
            }
        });
        map.load(mockfs[`style_${type}`]);
        map.render({ zoom: 16 }, function(err, pixels) {
            t.error(err);
            t.assert(pixels);
            t.end();
        });
    });

    test(`render reports an error when the request function responds with an error for a tile (${type})`, function(t) {
        var map = new mbgl.Map({
            request: function(req, callback) {
                var data = mockfs.dataForRequest(req);
                if (mockfs[`tile_${type}`] === data) {
                    callback(new Error('message'));
                } else {
                    callback(null, { data: data });
                }
            }
        });
        map.load(mockfs[`style_${type}`]);
        map.render({ zoom: 16 }, function(err, pixels) {
            t.assert(err);
            t.assert(/message/.test(err.message));
            t.assert(!pixels);
            t.end();
        });
    });
});
