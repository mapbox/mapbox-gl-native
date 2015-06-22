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

function setup(fileSource, callback) {
    callback(new mbgl.Map(fileSource));
}

test('Map', function(t) {
    t.test('constructor', function(t) {
        t.test('must be called with new', function(t) {
            t.throws(function() {
                mbgl.Map();
            }, /Use the new operator to create new Map objects/);

            t.end();
        });

        t.test('should require a FileSource object as first parameter', function(t) {
            t.throws(function() {
                new mbgl.Map();
            }, /Requires a FileSource as first argument/);

            t.throws(function() {
                new mbgl.Map('fileSource');
            }, /Requires a FileSource as first argument/);

            t.throws(function() {
                new mbgl.Map({});
            }, /Requires a FileSource as first argument/);

            t.end();
        });

        t.test('should require the FileSource object to have request and cancel methods', function(t) {
            var fileSource = new mbgl.FileSource();

            t.throws(function() {
                new mbgl.Map(fileSource);
            }, /FileSource must have a request member function/);

            fileSource.request = 'test';
            t.throws(function() {
                new mbgl.Map(fileSource);
            }, /FileSource must have a request member function/);

            fileSource.request = function() {};
            t.throws(function() {
                new mbgl.Map(fileSource);
            }, /FileSource must have a cancel member function/);

            fileSource.cancel = 'test';
            t.throws(function() {
                new mbgl.Map(fileSource);
            }, /FileSource must have a cancel member function/);

            fileSource.cancel = function() {};
            t.doesNotThrow(function() {
                new mbgl.Map(fileSource);
            });

            t.end();
        });

        t.end();
    });

    t.test('load styles', function(t) {
        var fileSource = new mbgl.FileSource();
        fileSource.request = function() {};
        fileSource.cancel = function() {};

        t.test('requires a string or object as the first parameter', function(t) {
            t.test('requires a map style as first argument', function(t) {
                setup(fileSource, function(map) {
                    t.throws(function() {
                        map.load();
                    }, /Requires a map style as first argument/);

                    map.release();

                    t.end();
                });
            });

            t.test('expect either an object or array at root', { timeout: 1000 }, function(t) {
                setup(fileSource, function(map) {
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
            setup(fileSource, function(map) {
                t.doesNotThrow(function() {
                    map.load('{}');
                });

                map.release();

                t.end();
            });
        });

        t.test('accepts a JSON stylesheet', { timeout: 1000 }, function(t) {
            setup(fileSource, function(map) {
                t.doesNotThrow(function() {
                    map.load(style);
                });

                map.release();

                t.end();
            });
        });

        t.test('accepts a stringified stylesheet', { timeout: 1000 }, function(t) {
            setup(fileSource, function(map) {
                t.doesNotThrow(function() {
                    map.load(JSON.stringify(style));
                });

                map.release();

                t.end();
            });
        });

        t.end();
    });

    t.test('request coalesting', function(t) {
        var requestList = [];
        var fileSourceDoNotReply = new mbgl.FileSource();

        // This file source will never reply to any
        // request other than "./fixtures/tiles.tilejson" which
        // will force the cancellation of the pending requests.
        fileSourceDoNotReply.request = function(req) {
            if (req.url != "./fixtures/tiles.tilejson") {
                requestList.push(req);
                return;
            }

            fs.readFile(path.join('test', req.url), function(err, data) {
                req.respond(err, { data: data });
            });
        };

        fileSourceDoNotReply.cancel = function() {
            requestList.pop();
        };

        t.test('merge requests/cancels for the same resource', function(t) {
            var map1 = new mbgl.Map(fileSourceDoNotReply);
            var map2 = new mbgl.Map(fileSourceDoNotReply);
            var map3 = new mbgl.Map(fileSourceDoNotReply);
            var map4 = new mbgl.Map(fileSourceDoNotReply);
            var map5 = new mbgl.Map(fileSourceDoNotReply);

            map1.load(style);
            map2.load(style);
            map3.load(style);
            map4.load(style);
            map5.load(style);

            var renderCallback = function(err, data) {}

            map1.render({}, renderCallback);
            map2.render({}, renderCallback);
            map3.render({}, renderCallback);
            map4.render({}, renderCallback);
            map5.render({}, renderCallback);

            var checkCancel = function() {
                t.equal(requestList.length, 0);
                t.end();
            };

            var checkCoalescing = function() {
                // Check if all the 5 vector tile
                // requests are going to merge in
                // a single request.
                t.equal(requestList.length, 1);

                // This should cause the pending
                // resources to get canceled, but
                // only one cancel is emitted, because
                // all map objects requested the same
                // resource.
                map1.release();
                map2.release();
                map3.release();
                map4.release();
                map5.release();

                setTimeout(checkCancel, 500);
            };

            setTimeout(checkCoalescing, 500);
        });

        var vectorTileRequestCount = 0;
        var fileSourceDelay = new mbgl.FileSource();

        // This file source will add a little delay before
        // replying so we make sure that all the map objects
        // request the same resource will get a reply from the
        // same request.
        fileSourceDelay.request = function(req) {
            var timeout = 0;

            if (req.url != "./fixtures/tiles.tilejson") {
                timeout = 500;
                t.ok(++vectorTileRequestCount == 1, "Should make only one request for tiles");
            }

            var readFile = function() {
                fs.readFile(path.join('test', req.url), function(err, data) {
                    req.respond(err, { data: data });
                });
            };

            setTimeout(readFile, timeout);
        };

        fileSourceDelay.cancel = function() {
            t.fail("Should never cancel");
        };

        t.test('one request for the same resource notifies multiple map objects', function(t) {
            var map1 = new mbgl.Map(fileSourceDelay);
            var map2 = new mbgl.Map(fileSourceDelay);
            var map3 = new mbgl.Map(fileSourceDelay);
            var map4 = new mbgl.Map(fileSourceDelay);
            var map5 = new mbgl.Map(fileSourceDelay);

            map1.load(style);
            map2.load(style);
            map3.load(style);
            map4.load(style);
            map5.load(style);

            var renderFinishedCount = 0;
            var renderCallback = function(err, data) {
                // We make only one request for tiles, but that
                // should be enough for all these map objects because
                // they all want the same resource.
                if (++renderFinishedCount == 5) {
                    map1.release();
                    map2.release();
                    map3.release();
                    map4.release();
                    map5.release();

                    t.end();
                }
            }

            map1.render({}, renderCallback);
            map2.render({}, renderCallback);
            map3.render({}, renderCallback);
            map4.render({}, renderCallback);
            map5.render({}, renderCallback);
        });

        t.end();
    });

    t.test('render argument requirements', function(t) {
        var fileSource = new mbgl.FileSource();
        fileSource.request = function(req) {
            fs.readFile(path.join('test', req.url), function(err, data) {
                req.respond(err, { data: data });
            });
        };
        fileSource.cancel = function() {};

        t.test('requires an object as the first parameter', function(t) {
            setup(fileSource, function(map) {
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
            setup(fileSource, function(map) {
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
            setup(fileSource, function(map) {
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

            setup(fileSource, function(map) {
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
            setup(fileSource, function(map) {
                map.release();

                t.throws(function() {
                    map.release();
                }, /Map resources have already been released/);

                t.end();
            });
        });

        t.test('returns an image', function(t) {
            setup(fileSource, function(map) {
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
