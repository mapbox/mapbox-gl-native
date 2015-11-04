'use strict';

var test = require('tape');
var mbgl = require('../../../../lib/mapbox-gl-native');
var fs = require('fs');
var path = require('path');
var style = require('../fixtures/style.json');

test('Map', function(t) {
    t.test('must be constructed with new', function(t) {
        t.throws(function() {
            mbgl.Map();
        }, /Use the new operator to create new Map objects/);

        t.end();
    });

    t.test('must be constructed with options object', function(t) {
        t.throws(function() {
            new mbgl.Map();
        }, /Requires an options object as first argument/);

        t.throws(function() {
            new mbgl.Map('options');
        }, /Requires an options object as first argument/);

        t.end();
    });

    t.test('requires request property', function(t) {
        var options = {};

        t.throws(function() {
            new mbgl.Map(options);
        }, /Options object must have a 'request' method/);

        options.request = 'test';
        t.throws(function() {
            new mbgl.Map(options);
        }, /Options object must have a 'request' method/);

        options.request = function() {};
        t.doesNotThrow(function() {
            new mbgl.Map(options);
        });

        t.end();
    });

    t.test('optional cancel property must be a function', function(t) {
        var options = {
            request: function() {}
        };

        options.cancel = 'test';
        t.throws(function() {
            new mbgl.Map(options);
        }, /Options object 'cancel' property must be a function/);

        options.cancel = function() {};
        t.doesNotThrow(function() {
            new mbgl.Map(options);
        });

        t.end();
    });


    t.test('optional ratio property must be a number', function(t) {
        var options = {
            request: function() {}
        };

        options.ratio = 'test';
        t.throws(function() {
            new mbgl.Map(options);
        }, /Options object 'ratio' property must be a number/);

        options.ratio = 1.0;
        t.doesNotThrow(function() {
            new mbgl.Map(options);
        });

        t.end();
    });

    t.test('.load', function(t) {
        var options = {
            request: function() {},
            ratio: 1
        };

        t.test('requires a map style as first argument', function(t) {
            var map = new mbgl.Map(options);

            t.throws(function() {
                map.load();
            }, /Requires a map style as first argument/);

            map.release();
            t.end();
        });

        t.test('expect either an object or array at root', { timeout: 1000 }, function(t) {
            var map = new mbgl.Map(options);

            mbgl.once('message', function(msg) {
                t.equal(msg.severity, 'ERROR');
                t.equal(msg.class, 'ParseStyle');
                t.ok(msg.text.match(/Invalid value/));

                map.release();
                t.end();
            });

            map.load('invalid');
        });

        t.test('accepts an empty stylesheet string', function(t) {
            var map = new mbgl.Map(options);

            t.doesNotThrow(function() {
                map.load('{}');
            });

            map.release();
            t.end();
        });

        t.test('accepts a JSON stylesheet', { timeout: 1000 }, function(t) {
            var map = new mbgl.Map(options);

            t.doesNotThrow(function() {
                map.load(style);
            });

            map.release();
            t.end();
        });

        t.test('accepts a stringified stylesheet', { timeout: 1000 }, function(t) {
            var map = new mbgl.Map(options);

            t.doesNotThrow(function() {
                map.load(JSON.stringify(style));
            });

            map.release();
            t.end();
        });

        t.test('does not immediately trigger any tile loads', function(t) {
            var map = new mbgl.Map({
                request: function(req) {
                    t.fail('unexpected request ' + req.url);
                },
                ratio: 1
            });

            map.load(style);

            setTimeout(function() {
                map.release();
                t.end();
            }, 100);
        });
    });

    t.test('.render', function(t) {
        var options = {
            request: function(req, callback) {
                fs.readFile(path.join(__dirname, '..', req.url), function(err, data) {
                    callback(err, { data: data });
                });
            },
            ratio: 1
        };

        t.test('requires an object as the first parameter', function(t) {
            var map = new mbgl.Map(options);

            t.throws(function() {
                map.render();
            }, /First argument must be an options object/);

            t.throws(function() {
                map.render('invalid');
            }, /First argument must be an options object/);

            map.release();
            t.end();
        });

        t.test('requires a callback as the second parameter', function(t) {
            var map = new mbgl.Map(options);

            t.throws(function() {
                map.render({});
            }, /Second argument must be a callback function/);

            t.throws(function() {
                map.render({}, 'invalid');
            }, /Second argument must be a callback function/);

            map.release();
            t.end();
        });

        t.test('requires a style to be set', function(t) {
            var map = new mbgl.Map(options);

            t.throws(function() {
                map.render({}, function() {});
            }, /Style is not loaded/);

            map.release();
            t.end();
        });

        t.test('returns an error', function(t) {
            mbgl.on('message', function(msg) {
                t.ok(msg, 'emits error');
                t.equal(msg.class, 'Style');
                t.equal(msg.severity, 'ERROR');
                t.ok(msg.text.match(/Failed to load/), 'error text matches');
            });

            var map = new mbgl.Map(options);
            map.load(style);
            map.render({ zoom: 1 }, function(err, data) {
                mbgl.removeAllListeners('message');
                map.release();

                t.ok(err, 'returns error');
                t.ok(err.message.match(/Failed to load/), 'error text matches');

                t.end();
            });
        });

        t.test('double release', function(t) {
            var map = new mbgl.Map(options);
            map.release();

            t.throws(function() {
                map.release();
            }, /Map resources have already been released/);

            t.end();
        });

        t.test('returns an image', function(t) {
            var map = new mbgl.Map(options);
            map.load(style);
            map.render({}, function(err, pixels) {
                t.error(err);
                map.release();
                t.ok(pixels);
                t.ok(pixels instanceof Buffer);
                t.equal(pixels.length, 512 * 512 * 4)
                t.end();
            });
        });

        t.test('can be called several times in serial', function(t) {
            var completed = 0;
            var remaining = 10;
            var start = +new Date;

            var map = new mbgl.Map(options);
            map.load(style);

            function render() {
                map.render({}, function(err, data) {
                    t.error(err);

                    t.ok(true, 'render @ ' + ((+new Date) - start) + 'ms');
                    if (++completed === remaining) {
                        map.release();
                        t.end();
                    } else {
                        render();
                    }
                });
            }

            render();
        });

        t.test('throws if called in parallel', function(t) {
            var map = new mbgl.Map(options);
            map.load(style);

            t.throws(function() {
                map.render({}, function() {});
                map.render({}, function() {});
            }, /Map is currently rendering an image/);

            map.release();
            t.end();
        });
    });
});
