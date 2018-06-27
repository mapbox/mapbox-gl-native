import {run} from '../../../mapbox-gl-js/test/integration/lib/query';
import implementation from './suite_implementation';
import ignores from './ignores.json';

let tests;

if (process.argv[1] === __filename && process.argv.length > 2) {
    tests = process.argv.slice(2);
}

run('native', {tests, ignores}, implementation);
