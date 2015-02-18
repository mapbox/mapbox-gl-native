'use strict';

/* jshint node: true */

var test = require('tape');
var mbgl = require('../..');

test('FileSource', function(t) {
    t.test('constructor', function(t) {
        t.test('must be called with new', function(t) {
            t.throws(function() {
                mbgl.FileSource();
            }, /Use the new operator to create new FileSource objects/);
            t.end();
        });

        t.end();
    });

    t.end();
});
