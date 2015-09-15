'use strict';

/* jshint node: true */

var test = require('tape');
var mbgl = require('../../../..');

test('View', function(t) {
    t.test('constructor', function(t) {
        t.test('must be called with new', function(t) {
            t.throws(function() {
                mbgl.View();
            }, /Use the new operator to create new View objects/);

            t.end();
        });

        t.test('should require an options object as first parameter', function(t) {
            t.throws(function() {
                new mbgl.View();
            }, /Requires an options object as first argument/);

            t.throws(function() {
                new mbgl.View('options');
            }, /Requires an options object as first argument/);

            t.end();
        });

        t.test('should require the options object to have pixel ratio, width and height attributes', function(t) {
            var options = {};

            t.throws(function() {
                new mbgl.View(options);
            }, /must have a numerical 'width' property/);

            options.width = 'test';
            t.throws(function() {
                new mbgl.View(options);
            }, /must have a numerical 'width' property/);

            options.width = 512;
            t.throws(function() {
                new mbgl.View(options);
            }, /must have a numerical 'height' property/);

            options.height = 'test';
            t.throws(function() {
                new mbgl.View(options);
            }, /must have a numerical 'height' property/);

            options.height = 512;
            t.throws(function() {
                new mbgl.View(options);
            }, /must have a numerical 'ratio' property/);

            options.ratio = 'test';
            t.throws(function() {
                new mbgl.View(options);
            }, /must have a numerical 'ratio' property/);

            options.ratio = 1.0;
            t.doesNotThrow(function() {
                new mbgl.View(options);
            });

            t.end();
        });

        t.end();
    });

    t.test('can be passed to Map constructor', function(t) {
        var options = {};
        options.request = function() {};
        options.cancel = function() {};

        options.view = new mbgl.View({
            width: 512,
            height: 512,
            ratio: 1.0
        });

        t.doesNotThrow(function() {
            new mbgl.Map(options);
        });

        t.end();
    });

    t.end();
});
