"use strict";

var fs = require('fs');
var path = require('path');

function readFixture(file) {
    return fs.readFileSync(path.join('test/fixtures/resources', file));
}

var style_raster  = readFixture('style_raster.json').toString('utf8');
var style_vector  = readFixture('style_vector.json').toString('utf8');
var sprite_json   = readFixture('sprite.json');
var sprite_png    = readFixture('sprite.png');
var glyph         = readFixture('glyphs.pbf');
var source_raster = readFixture('source_raster.json');
var source_vector = readFixture('source_vector.json');
var tile_raster   = readFixture('raster.tile');
var tile_vector   = readFixture('vector.tile');

function dataForRequest(req) {
    if (req.url === null) {
        return null;
    } else if (req.url.indexOf('sprite') > -1 && req.url.endsWith('json')) {
        return sprite_json;
    } else if (req.url.indexOf('sprite') > -1 && req.url.endsWith('png')) {
        return sprite_png;
    } else if (req.url.indexOf('fonts') > -1 && req.url.endsWith('pbf')) {
        return glyph;
    } else if (req.url.endsWith('mapbox.satellite')) {
        return source_raster;
    } else if (req.url.indexOf('satellite') > -1 && (req.url.endsWith('png') || req.url.endsWith('webp'))) {
        return tile_raster;
    } else if (req.url.endsWith('mapbox.mapbox-streets-v7')) {
        return source_vector;
    } else if (req.url.indexOf('streets') > -1 && req.url.endsWith('pbf')) {
        return tile_vector;
    } else {
        return null;
    }
}

module.exports = {
    dataForRequest: dataForRequest,
    style_raster: style_raster,
    style_vector: style_vector,
    sprite_json: sprite_json,
    sprite_png: sprite_png,
    glyph: glyph,
    source_raster: source_raster,
    source_vector: source_vector,
    tile_raster: tile_raster,
    tile_vector: tile_vector
};
