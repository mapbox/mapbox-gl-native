"use strict";

// Shim to wrap req.respond while preserving callback-passing API

var mbgl = require('../../lib/mapbox_gl_native.node');
var constructor = mbgl.Map.prototype.constructor;
var process = require('process');

var Map = function(options) {
    if (!(options instanceof Object)) {
        throw TypeError("Requires an options object as first argument");
    }

    if (!options.hasOwnProperty('request') || !(options.request instanceof Function)) {
        throw TypeError("Options object must have a 'request' method");
    }

    var request = options.request;

    return new constructor(Object.assign(options, {
        request: function(req) {
            // Protect against `request` implementations that call the callback synchronously,
            // call it multiple times, or throw exceptions.
            // http://blog.izs.me/post/59142742143/designing-apis-for-asynchrony

            var responded = false;
            var callback = function() {
                var args = arguments;
                if (!responded) {
                    responded = true;
                    process.nextTick(function() {
                        req.respond.apply(req, args);
                    });
                } else {
                    console.warn('request function responded multiple times; it should call the callback only once');
                }
            };

            try {
                request(req, callback);
            } catch (e) {
                console.warn('request function threw an exception; it should call the callback with an error instead');
                callback(e);
            }
        }
    }));
};

Map.prototype = mbgl.Map.prototype;
Map.prototype.constructor = Map;

module.exports = Object.assign(mbgl, { Map: Map });
