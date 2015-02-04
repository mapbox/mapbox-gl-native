/* jshint node: true, unused: false */
/* global describe, it, beforeEach, afterEach */
'use strict';
var assert = require('assert');

var mbgl = require('..');

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
            var fs = new mbgl.FileSource();

            assert.throws(function() {
                new mbgl.Map(fs);
            }, /FileSource must have a request member function/);

            fs.request = 'test';
            assert.throws(function() {
                new mbgl.Map(fs);
            }, /FileSource must have a request member function/);

            fs.request = function() {};
            assert.throws(function() {
                new mbgl.Map(fs);
            }, /FileSource must have a cancel member function/);

            fs.cancel = 'test';
            assert.throws(function() {
                new mbgl.Map(fs);
            }, /FileSource must have a cancel member function/);

            fs.cancel = function() {};
            assert.doesNotThrow(function() {
                new mbgl.Map(fs);
            });
        });

    });

    describe('load styles', function() {
        var map;

        beforeEach(function() {
            var fs = new mbgl.FileSource();
            fs.request = function() {};
            fs.cancel = function() {};
            map = new mbgl.Map(fs);
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

        it('accepts a stylesheet string', function() {
            map.load('{}');
        });

    });


    describe('render arguments', function() {
        var map;

        beforeEach(function() {
            var fs = new mbgl.FileSource();
            fs.request = function() {};
            fs.cancel = function() {};
            map = new mbgl.Map(fs);
        });

        afterEach(function() {
            map = null;
        });

        it('requires a string or object as the first parameter', function() {
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

        it('requires a callback as the second parameter', function(done) {
            map.render({}, function(err) {
                done(err);
            });
        });

    });

});
