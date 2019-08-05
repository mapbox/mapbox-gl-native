import {run} from '../../../mapbox-gl-js/test/integration/lib/render';
import implementation from './suite_implementation';
import ignores from './ignores.json';

let platformIgnore = {};
try {
    platformIgnore = require('./ignores-' + process.platform + '.json');
} catch (ex) {
    console.error(ex);
}

run('native', Object.assign({}, ignores, platformIgnore), implementation);
