'use strict';

var mbgl = require('../../..');
var suite = require('mapbox-gl-test-suite');
var request = require('request');

var tests;

if (process.argv[1] === __filename && process.argv.length > 2) {
    tests = process.argv.slice(2);
}

suite.run('native', {tests: tests}, function (style, options, callback) {
    var map = new mbgl.Map({
        ratio: options.pixelRatio,
        request: function (req) {
            request(req.url, {encoding: null}, function (err, response, body) {
                req.respond(err, {data: body});
            });
        }
    });

    map.load(style);
    map.render(options, function (err, pixels) {
        map.release();
        callback(err, pixels);
    });
});
