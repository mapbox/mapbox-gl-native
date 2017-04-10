'use strict';

var mockfs = require('../mockfs');
var mbgl = require('../../index');
var tape = require('tape');
var timeout = 2000;

function asyncReply(callback, data) {
    setTimeout(function() { callback(null, { data: data }); }, 0);
};

function timeoutRequest(t, style, timeoutedResource) {
    var options = {
        request: function(req, callback) {
            var data = mockfs.dataForRequest(req);

            if (timeoutedResource != data) {
                asyncReply(callback, data);
            } else {
                setTimeout(function() { callback(null, { data: data }); t.pass("callback"); }, timeout);
            }
        },
        ratio: 2,
    };

    var map = new mbgl.Map(options);
    map.load(style);

    map.render({ zoom: 16 }, function(err, pixels) {
        t.fail("fail");
    });

    setTimeout(function() {
        map.release();
        t.pass("release");
    }, timeout / 2);
};

tape.skip('Vector', function(t) {
    // 1 sprite.png
    // 1 spring.json
    // 1 source
    // 4 tiles
    // 7 glyphs
    // 5 map.release()
    t.plan(19);

    timeoutRequest(t, mockfs.style_vector, mockfs.sprite_png);
    timeoutRequest(t, mockfs.style_vector, mockfs.sprite_json);
    timeoutRequest(t, mockfs.style_vector, mockfs.source_vector);
    timeoutRequest(t, mockfs.style_vector, mockfs.tile_vector);
    timeoutRequest(t, mockfs.style_vector, mockfs.glyph);
});

tape.skip('Raster', function(t) {
    // 1 sprite.png
    // 1 spring.json
    // 1 source
    // 4 tiles
    // 4 map.release()
    t.plan(11);

    timeoutRequest(t, mockfs.style_raster, mockfs.sprite_png);
    timeoutRequest(t, mockfs.style_raster, mockfs.sprite_json);
    timeoutRequest(t, mockfs.style_raster, mockfs.source_raster);
    timeoutRequest(t, mockfs.style_raster, mockfs.tile_raster);
});
