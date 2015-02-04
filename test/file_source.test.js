/* jshint node: true, unused: false */
/* global describe, it */
'use strict';
var assert = require('assert');

var mbgl = require('..');

describe('FileSource', function() {

    describe('constructor', function() {
        it('must be called with new', function() {
            assert.throws(function() {
                mbgl.FileSource();
            }, /Use the new operator to create new FileSource objects/);
        });

    });

});
