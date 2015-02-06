/* jshint node: true, unused: false */
/* global describe, it, beforeEach, afterEach */
'use strict';
var assert = require('assert');

var mbgl = require('..');
var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');

mkdirp.sync('test/results');

var style = require('./fixtures/style.json');

describe('Map', function() {

    describe('constructor', function() {
        it('must be called with new', function() {
            assert.throws(function() {
                mbgl.Map();
            }, /Use the new operator to create new Map objects/);
        });

        it('should require a FileSource object as first parameter', function() {
            assert.throws(function() {
                new mbgl.Map();
            }, /Requires a FileSource as first argument/);

            assert.throws(function() {
                new mbgl.Map('fileSource');
            }, /Requires a FileSource as first argument/);

            assert.throws(function() {
                new mbgl.Map({});
            }, /Requires a FileSource as first argument/);
        });

        it('should require the FileSource object to have request and cancel methods', function() {
            var fileSource = new mbgl.FileSource();

            assert.throws(function() {
                new mbgl.Map(fileSource);
            }, /FileSource must have a request member function/);

            fileSource.request = 'test';
            assert.throws(function() {
                new mbgl.Map(fileSource);
            }, /FileSource must have a request member function/);

            fileSource.request = function() {};
            assert.throws(function() {
                new mbgl.Map(fileSource);
            }, /FileSource must have a cancel member function/);

            fileSource.cancel = 'test';
            assert.throws(function() {
                new mbgl.Map(fileSource);
            }, /FileSource must have a cancel member function/);

            fileSource.cancel = function() {};
            assert.doesNotThrow(function() {
                new mbgl.Map(fileSource);
            });
        });

    });

    describe('load styles', function() {
        var map;

        var fileSource = new mbgl.FileSource();
        fileSource.request = function() {};
        fileSource.cancel = function() {};

        beforeEach(function() {
            map = new mbgl.Map(fileSource);
        });

        afterEach(function() {
            map = null;
        });

        it('requires a string or object as the first parameter', function() {
            assert.throws(function() {
                map.load();
            }, /Requires a map style as first argument/);

            assert.throws(function() {
                map.load('invalid');
            }, /Expect either an object or array at root/);
        });

        it('accepts an empty stylesheet string', function() {
            map.load('{}');
        });

        it('accepts a JSON stylesheet', function() {
            map.load(style);
        });

        it('accepts a stringified stylesheet', function() {
            map.load(JSON.stringify(style));
        });
    });

    describe('render argument requirements', function() {
        var map;

        var fileSource = new mbgl.FileSource();
        fileSource.request = function(req) {
            fs.readFile(path.join('test', req.url), function(err, data) {
                req.respond(err, { data: data });
                assert.ifError(err);
            });
        };
        fileSource.cancel = function() {};

        beforeEach(function() {
            map = new mbgl.Map(fileSource);
        });

        afterEach(function() {
            map = null;
        });

        it('requires an object as the first parameter', function() {
            assert.throws(function() {
                map.render();
            }, /First argument must be an options object/);

            assert.throws(function() {
                map.render('invalid');
            }, /First argument must be an options object/);
        });

        it('requires a callback as the second parameter', function() {
            assert.throws(function() {
                map.render({});
            }, /Second argument must be a callback function/);

            assert.throws(function() {
                map.render({}, 'invalid');
            }, /Second argument must be a callback function/);
        });

        it('requires a style to be set', function(done) {
            map.render({}, function(err) {
                assert.ok(err);
                assert.equal(err.message, 'Style is not set');
                done();
            });
        });

        it('returns an image', function(done) {
            map.load(style);
            map.render({}, function(err, data) {
                assert.ifError(err);
                fs.writeFileSync('test/results/image.png', data);
                done();
            });
        });
    });

});
