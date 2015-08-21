'use strict';

/* jshint node: true */

var test = require('tape');
var mbgl = require('../..');
var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');
var style = require('../fixtures/style.json');
var PNG = require('pngjs').PNG;
var compare = require('../compare.js');

function filePath(name) {
    return ['expected', 'actual', 'diff'].reduce(function(prev, key) {
        var dir = path.join('test', key, 'map');
        mkdirp.sync(dir);
        prev[key] = path.join(dir, name);
        return prev;
    }, {});
}

function setup(options, callback) {
    callback(new mbgl.Map(options));
}

test('Map', function(t) {
    t.test('constructor', function(t) {
        t.test('must be called with new', function(t) {
            t.throws(function() {
                mbgl.Map();
            }, /Use the new operator to create new Map objects/);

            t.end();
        });

        t.test('should require an options object as first parameter', function(t) {
            t.throws(function() {
                new mbgl.Map();
            }, /Requires an options object as first argument/);

            t.throws(function() {
                new mbgl.Map('options');
            }, /Requires an options object as first argument/);

            t.end();
        });

        t.test('should require then options object to have request and cancel methods', function(t) {
            var options = {};

            t.throws(function() {
                new mbgl.Map(options);
            }, /Options object must have a 'request' method/);

            options.request = 'test';
            t.throws(function() {
                new mbgl.Map(options);
            }, /Options object must have a 'request' method/);

            options.request = function() {};
            options.cancel = 'test';
            t.throws(function() {
                new mbgl.Map(options);
            }, /Options object 'cancel' property must be a function/);

            options.cancel = function() {};
            t.throws(function() {
                new mbgl.Map(options);
            }, /Options object must have a numerical 'ratio' property/);

            options.ratio = 'test';
            t.throws(function() {
                new mbgl.Map(options);
            }, /Options object must have a numerical 'ratio' property/);

            options.ratio = 1.0;
            t.doesNotThrow(function() {
                new mbgl.Map(options);
            });

            t.end();
        });

        t.end();
    });

    t.test('load styles', function(t) {
        var options = {};
        options.request = function() {};
        options.cancel = function() {};
        options.ratio = 1.0;

        t.test('requires a string or object as the first parameter', function(t) {
            t.test('requires a map style as first argument', function(t) {
                setup(options, function(map) {
                    t.throws(function() {
                        map.load();
                    }, /Requires a map style as first argument/);

                    map.release();

                    t.end();
                });
            });

            t.test('expect either an object or array at root', { timeout: 1000 }, function(t) {
                setup(options, function(map) {
                    mbgl.once('message', function(msg) {
                        t.equal(msg.severity, 'ERROR');
                        t.equal(msg.class, 'ParseStyle');
                        t.ok(msg.text.match(/Expect either an object or array at root/));

                        map.release();

                        t.end();
                    });

                    map.load('invalid');
                });
            });

            t.end();
        });

        t.test('accepts an empty stylesheet string', function(t) {
            setup(options, function(map) {
                t.doesNotThrow(function() {
                    map.load('{}');
                });

                map.release();

                t.end();
            });
        });

        t.test('accepts a JSON stylesheet', { timeout: 1000 }, function(t) {
            setup(options, function(map) {
                t.doesNotThrow(function() {
                    map.load(style);
                });

                map.release();

                t.end();
            });
        });

        t.test('accepts a stringified stylesheet', { timeout: 1000 }, function(t) {
            setup(options, function(map) {
                t.doesNotThrow(function() {
                    map.load(JSON.stringify(style));
                });

                map.release();

                t.end();
            });
        });

        t.end();
    });

    t.test('render argument requirements', function(t) {
        var options = {};
        options.request = function(req) {
            fs.readFile(path.join('test', req.url), function(err, data) {
                req.respond(err, { data: data });
            });
        };
        options.cancel = function() {};
        options.ratio = 1.0;

        t.test('requires an object as the first parameter', function(t) {
            setup(options, function(map) {
                t.throws(function() {
                    map.render();
                }, /First argument must be an options object/);

                t.throws(function() {
                    map.render('invalid');
                }, /First argument must be an options object/);

                map.release();

                t.end();
            });
        });

        t.test('requires a callback as the second parameter', function(t) {
            setup(options, function(map) {
                t.throws(function() {
                    map.render({});
                }, /Second argument must be a callback function/);

                t.throws(function() {
                    map.render({}, 'invalid');
                }, /Second argument must be a callback function/);

                map.release();

                t.end();
            });
        });

        t.test('requires a style to be set', function(t) {
            setup(options, function(map) {
                t.throws(function() {
                    map.render({}, function() {});
                }, /Style is not loaded/);

                map.release();

                t.end();
            });
        });

        t.test('returns an error', function(t) {
            mbgl.on('message', function(msg) {
                t.ok(msg, 'emits error');
                t.equal(msg.class, 'Style');
                t.equal(msg.severity, 'ERROR');
                t.ok(msg.text.match(/Failed to load/), 'error text matches');
            });

            setup(options, function(map) {
                map.load(style);
                map.render({ zoom: 1 }, function(err, data) {
                    mbgl.removeAllListeners('message');
                    map.release();

                    t.ok(err, 'returns error');
                    t.ok(err.message.match(/Failed to load/), 'error text matches');

                    t.end();
                });
            });
        });

        t.test('double release', function(t) {
            setup(options, function(map) {
                map.release();

                t.throws(function() {
                    map.release();
                }, /Map resources have already been released/);

                t.end();
            });
        });

        t.test('returns an image', function(t) {
            setup(options, function(map) {
                map.load(style);
                map.render({}, function(err, data) {
                    t.error(err);

                    map.release();

                    var filename = filePath('image.png');

                    var png = new PNG({
                        width: data.width,
                        height: data.height
                    });

                    png.data = data.pixels;

                    if (process.env.UPDATE) {
                        png.pack()
                            .pipe(fs.createWriteStream(filename.expected))
                            .on('finish', t.end);
                    } else {
                        png.pack()
                            .pipe(fs.createWriteStream(filename.actual))
                            .on('finish', function() {
                                compare(filename.actual, filename.expected, filename.diff, t, function(err, diff) {
                                    t.error(err);
                                    t.ok(diff <= 0.01, 'actual matches expected');
                                    t.end();
                                });
                            });
                    }
                });
            });
        });

        t.end();
    });

    t.end();
});
