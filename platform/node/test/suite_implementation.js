'use strict';

var mbgl = require('../../../lib/mapbox-gl-native');
var request = require('request');

mbgl.on('message', function(msg) {
    console.log('%s (%s): %s', msg.severity, msg.class, msg.text);
});

module.exports = function (style, options, callback) {
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
    options.debug = {
        tileBorders: options.debug,
        collision: options.collisionDebug
    };

    map.load(style);

    map.render(options, function (err, pixels) {
        var results = options.queryGeometry ?
            map.queryRenderedFeatures(options.queryGeometry) :
            [];
        map.release();
        if (timedOut) return;
        clearTimeout(watchdog);
        callback(err, pixels, results.map(prepareFeatures));
    });

    function prepareFeatures(json) {
        var r = JSON.parse(json);
        delete r.layer;
        r.geometry = null;
        return r;
    }
};
