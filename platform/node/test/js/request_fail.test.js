'use strict';

var mockfs = require('../mockfs');
var mbgl = require('../../index');
var test = require('tape');

function asyncReply(callback, data) {
    setTimeout(function() { callback(null, { data: data }); }, 0);
};

function asyncFail(callback) {
    setTimeout(function() { callback(new Error('not found')); }, 0);
};

function failRequest(t, style, failedResource) {
    var options = {
        request: function(req, callback) {
            var data = mockfs.dataForRequest(req);

            if (failedResource != data) {
                asyncReply(callback, data);
            } else {
                asyncFail(callback);
            }
        },
        ratio: 2,
    };

    var map = new mbgl.Map(options);
    map.load(style);

    map.render({ zoom: 16 }, function(err, pixels) {
        if (err) {
            t.pass("pass");
            map.release();
        }
    });
};

test('Vector', function(t) {
    t.plan(5);

    failRequest(t, mockfs.style_vector, null);
    failRequest(t, mockfs.style_vector, mockfs.sprite_png);
    failRequest(t, mockfs.style_vector, mockfs.sprite_json);
    failRequest(t, mockfs.style_vector, mockfs.source_vector);
    failRequest(t, mockfs.style_vector, mockfs.tile_vector);
    failRequest(t, mockfs.style_vector, mockfs.glyph);
});

test('Raster', function(t) {
    t.plan(4);

    failRequest(t, mockfs.style_raster, null);
    failRequest(t, mockfs.style_raster, mockfs.sprite_png);
    failRequest(t, mockfs.style_raster, mockfs.sprite_json);
    failRequest(t, mockfs.style_raster, mockfs.source_raster);
    failRequest(t, mockfs.style_raster, mockfs.tile_raster);
});
