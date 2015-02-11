/* jshint node: true, unused: false */
'use strict';

var test = require('tape').test;

var mbgl = require('..');
var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');

mkdirp.sync('test/results');

var style = require('./fixtures/style.json');

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
            setup(fileSource, function(map) {
                t.throws(function() {
                    map.load();
                }, /Requires a map style as first argument/);

                t.throws(function() {
                    map.load('invalid');
                }, /Expect either an object or array at root/);
                t.end();
            });
        });

        t.test('accepts an empty stylesheet string', function(t) {
            setup(fileSource, function(map) {
                t.doesNotThrow(function() {
                    map.load('{}');
                });
                t.end();
            });
        });

        t.test('accepts a JSON stylesheet', function(t) {
            setup(fileSource, function(map) {
                t.doesNotThrow(function() {
                    map.load(style);
                });
                t.end();
            });
        });

        t.test('accepts a stringified stylesheet', function(t) {
            setup(fileSource, function(map) {
                t.doesNotThrow(function() {
                    map.load(JSON.stringify(style));
                });
                t.end();
            });
        });

        t.end();
    });

    t.test('render argument requirements', function(t) {
        var fileSource = new mbgl.FileSource();
        fileSource.request = function(req) {
            fs.readFile(path.join('test', req.url), function(err, data) {
                req.respond(err, { data: data });
                t.error(err);
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

                t.end();
            });
        });

        t.test('requires a style to be set', function(t) {
            setup(fileSource, function(map) {
                map.render({}, function(err) {
                    t.ok(err);
                    t.equal(err.message, 'Style is not set');
                    t.end();
                });
            });
        });

        t.test('returns an image', function(t) {
            setup(fileSource, function(map) {
                map.load(style);
                map.render({}, function(err, data) {
                    t.error(err);
                    fs.writeFileSync('test/results/image.png', data);
                    t.end();
                });
            });
        });
    });

    t.end();
});
