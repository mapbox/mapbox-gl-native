#!/usr/bin/env node
const nitpick = require('.');
const child_process = require('child_process');

// Make sure that the mapbox-gl-js submodule pin is up to date
const head = child_process.execSync('git -C mapbox-gl-js rev-parse HEAD').toString().trim();
const revs = child_process.execSync(`git -C mapbox-gl-js branch -a --contains ${head}`).toString().split('\n');

if (revs.indexOf('  remotes/origin/master') >= 0) {
    nitpick.ok(`mapbox-gl-js submodule pin is merged to master`);
} else {
    nitpick.fail(`mapbox-gl-js submodule is pinned to ${head}, which isn't merged to master`);
}
