#!/usr/bin/env node

const fs = require('fs');
const ejs = require('ejs');
const _ = require('lodash');
const semver = require('semver');

const changelog = fs.readFileSync('platform/ios/CHANGELOG.md', 'utf8');

// This regular expression parses the changelog for individual releases:
//  - Matches lines starting with "## ".
//  - Groups the version number.
//  - Skips the (optional) release date.
//  - Groups the changelog content.
//  - Ends when another "## " is found.
const regex = /^## (\d+\.\d+\.\d+).*?\n(.+?)(?=\n^## )/gms;

let releases = [];
while (match = regex.exec(changelog)) {
    releases.push({
        'version': match[1],
        'changelog': match[2].trim(),
    });
}

const currentRelease = releases[0];

const versions = _.map(releases, 'version');
const previousVersion = semver.maxSatisfying(versions, "<" + currentRelease.version);

const releaseNotes = ejs.render(fs.readFileSync('platform/ios/scripts/release-notes.md.ejs', 'utf8'), {
    'currentVersion': currentRelease.version,
    'previousVersion': previousVersion,
    'changelog': currentRelease.changelog,
});

process.stdout.write(releaseNotes);
