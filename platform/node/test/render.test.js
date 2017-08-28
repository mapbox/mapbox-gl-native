'use strict';

const suite = require('../../../mapbox-gl-js/test/integration').render;
const suiteImplementation = require('./suite_implementation');
const ignores = require('./ignores.json');

let tests;
let shuffle = false;
let recycleMap = false;

function checkParameter(param) {
    const index = tests.indexOf(param);
    if (index === -1)
        return false;
    tests.splice(index, 1);
    return true;
}

if (process.argv[1] === __filename && process.argv.length > 2) {
    tests = process.argv.slice(2).filter((value, index, self) => { return self.indexOf(value) === index; });
    shuffle = checkParameter('--shuffle');
    recycleMap = checkParameter('--recycle-map');
}

suite.run('native', {tests, ignores, shuffle, recycleMap}, suiteImplementation);
