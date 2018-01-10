// Import the feedback functions
import { message, warn, fail, markdown } from "danger"

const child_process = require('child_process');

// Make sure that the mapbox-gl-js submodule pin is up to date
const submodulePinsToMaster = child_process.execSync('git -C mapbox-gl-js branch -a --contains `git -C mapbox-gl-js rev-parse HEAD`').toString().split('\n').indexOf('  remotes/origin/master') >= 0;
if (!submodulePinsToMaster) {
    fail(`mapbox-gl-js submodule pin is not yet merged to master`);
}

const coreFileDiff = child_process.execSync('scripts/generate-core-files.sh').toString();
if (coreFileDiff != '') {
    fail(`Please run <code>scripts/generate-core-files.sh</code>:<pre lang="diff">${coreFileDiff}</pre>`)
}
