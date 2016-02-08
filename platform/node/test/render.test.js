'use strict';

var mbgl = require('../../../lib/mapbox-gl-native');
var suite = require('mapbox-gl-test-suite').render;
var request = require('request');

var tests;

if (process.argv[1] === __filename && process.argv.length > 2) {
    tests = process.argv.slice(2);
}

mbgl.on('message', function(msg) {
    console.log('%s (%s): %s', msg.severity, msg.class, msg.text);
});

suite.run('native', {tests: tests}, function (style, options, callback) {
    var map = new mbgl.Map({
        ratio: options.pixelRatio,
        request: function(req, callback) {
            request(req.url, {encoding: null}, function (err, response, body) {
                if (err) {
                    callback(err);
                } else if (response.statusCode != 200) {
                    callback(new Error(response.statusMessage));
                } else {
                    callback(null, {data: body});
                }
            });
        }
    });

    var timedOut = false;
    var watchdog = setTimeout(function () {
        timedOut = true;
        map.dumpDebugLogs();
        callback(new Error('timed out after 20 seconds'));
    }, 20000);

    options.center = style.center;
    options.zoom = style.zoom;
    options.bearing = style.bearing;
    options.pitch = style.pitch;

    map.load(style);
    map.render(options, function (err, pixels) {
        map.release();
        if (timedOut) return;
        clearTimeout(watchdog);
        callback(err, pixels);
    });
});
