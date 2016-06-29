// Shim to wrap req.respond while preserving callback-passing API

var mbgl = require('../../lib/mapbox-gl-native.node');
var Map = mbgl.Map;

module.exports = Object.assign(mbgl, {
    Map: function(options) {
        if (!(options instanceof Object)) {
            throw TypeError("Requires an options object as first argument");
        }

        if (!options.hasOwnProperty('request') || !(options.request instanceof Function)) {
            throw TypeError("Options object must have a 'request' method");
        }

        var request = options.request;

        return new Map(Object.assign(options, {
            request: function(req) {
                request(req, function() {
                    req.respond.apply(req, arguments);
                });
            }
        }));
    }
});
