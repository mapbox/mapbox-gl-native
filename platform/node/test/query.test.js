'use strict';

const suite = require('../../../mapbox-gl-js/test/integration').query;
const suiteImplementation = require('./suite_implementation');
const ignores = require('./ignores.json');

let tests;

if (process.argv[1] === __filename && process.argv.length > 2) {
    tests = process.argv.slice(2);
}

suite.run('native', {tests: tests, ignores: ignores}, suiteImplementation);
