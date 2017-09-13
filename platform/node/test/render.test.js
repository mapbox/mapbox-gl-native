'use strict';

const suite = require('../../../mapbox-gl-js/test/integration').render;
const suiteImplementation = require('./suite_implementation');
const ignores = require('./ignores.json');

suite.run('native', ignores, suiteImplementation);
