'use strict';

var suite = require('../../../mapbox-gl-js/test/integration').render;
var suiteImplementation = require('./suite_implementation');

var tests;

if (process.argv[1] === __filename && process.argv.length > 2) {
    tests = process.argv.slice(2);
}

suite.run('native', {tests: tests}, suiteImplementation);
