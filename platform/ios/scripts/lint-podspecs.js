#!/usr/bin/env node

const fs = require('fs');
const execSync = require('child_process').execSync;
const _ = require('lodash');
const semver = require('semver');

console.step = _.partial(console.log, '\n\033[1m\033[36m*', _, '\033[0m');
process.exitCode = 0;

const podspecsPath = 'platform/ios';

/*
    Step 1. Run the official CocoaPods linter against all of our podspecs.
*/
console.step(`Running CocoaPods linter against podspecs in '${podspecsPath}'`)
execSync(`pod spec lint ${podspecsPath}/*.podspec --quick`, {stdio: 'inherit'});

/*
    Step 2. Check that our podspecs contain properly formatted version strings.
*/
console.step('Checking Mapbox SDK version strings in podspecs')
const podspecs = fs.readdirSync(podspecsPath).filter(fn => fn.endsWith('.podspec'));

let matchedVersions = [];

for (const podspecFilename of podspecs) {
    console.log(podspecFilename);
    const podspecContents = fs.readFileSync(`${podspecsPath}/${podspecFilename}`, 'utf8');

    /*
      This regular expression:
        - Matches single lines in the format: version = '9.9.9'
        - Groups the version number inside the single quotes.
        - Ignores whitespace at the start of the line and between parts of the definition using: \s*?
    */
    const regex = /^\s*?version\s*?=\s*?'(.*)'$/gmi;

    const match = regex.exec(podspecContents);
    if (!match) {
        console.error('  ❌ No version string found');
        process.exitCode = 1;
        continue;
    }

    const matchedVersion = match[1];
    matchedVersions.push(matchedVersion);

    if (!semver.valid(matchedVersion)) {
        console.error(`  ❌ ${matchedVersion} is not a valid semantic version`);
        process.exitCode = 1;
        continue;
    }

    console.log(`  ✅ ${matchedVersion} is a valid semantic version`);
}

/*
    Step 3. Check that versions are consistent across our podspecs.
*/
console.step('Checking version consistency')
const uniqueVersions = _.uniq(matchedVersions);
if (uniqueVersions.length != 1) {
    console.error('❌ Versions in podspecs do not match:', uniqueVersions);
    process.exitCode = 1;
} else {
    console.log(`✅ ${uniqueVersions[0]} is set in all podspecs`);
}

/*
    Step 4. Finish up and summarize the results.
*/
process.on('exit', (code) => {
    if (code == 0) {
        console.log('\nPodspec linting completed successfully! 🕵️‍♀️');
    } else {
        console.log('\nPodspec linting failed. 💥');
    }
});
