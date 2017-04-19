'use strict';

var mockfs = require('../mockfs');
var mbgl = require('../../index');
var test = require('tape');

function isTile(data) {
    return data == mockfs.tile_vector || data == mockfs.tile_raster;
}

function asyncReply(callback, data) {
    setTimeout(function() { callback(null, { data: data }); }, 0);
};

function asyncFail(callback, data) {
    // Do not set an error for tile when not found. A not found
    // tile is a valid tile.
    if (isTile(data)) {
        setTimeout(function() { callback(); }, 0);
    } else {
        setTimeout(function() { callback(new Error('not found')); }, 0);
    }
};

function notfoundRequest(t, style, notfoundResource) {
    var options = {
        request: function(req, callback) {
            var data = mockfs.dataForRequest(req);

            if (notfoundResource != data) {
                asyncReply(callback, data);
            } else {
                asyncFail(callback, data);
            }
        },
        ratio: 2,
    };

    var map = new mbgl.Map(options);
    map.load(style);

    map.render({ zoom: 16 }, function(err, pixels) {
        if (err && !isTile(notfoundResource)) {
            t.pass("pass");
            return;
        }

        if (!err && isTile(notfoundResource)) {
            t.pass("pass");
            return;
        }

        t.fail("fail");
    });
};

test('Vector', function(t) {
    t.plan(5);

    notfoundRequest(t, mockfs.style_vector, mockfs.sprite_png);
    notfoundRequest(t, mockfs.style_vector, mockfs.sprite_json);
    notfoundRequest(t, mockfs.style_vector, mockfs.source_vector);
    notfoundRequest(t, mockfs.style_vector, mockfs.tile_vector);
    notfoundRequest(t, mockfs.style_vector, mockfs.glyph);
});

test('Raster', function(t) {
    t.plan(4);

    notfoundRequest(t, mockfs.style_raster, mockfs.sprite_png);
    notfoundRequest(t, mockfs.style_raster, mockfs.sprite_json);
    notfoundRequest(t, mockfs.style_raster, mockfs.source_raster);
    notfoundRequest(t, mockfs.style_raster, mockfs.tile_raster);
});
