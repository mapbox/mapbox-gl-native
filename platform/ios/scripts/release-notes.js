#!/usr/bin/env node

const fs = require('fs');
const child_process = require('child_process');
const ejs = require('ejs');
const _ = require('lodash');
const semver = require('semver');

const changelog = fs.readFileSync('platform/ios/CHANGELOG.md', 'utf8');

/*
  Find current and immediately previous releases by parsing git tags.
*/
let currentVersion = child_process.execSync('git describe --tags --match=ios-v*.*.* --abbrev=0')
    .toString()
    .trim()
    .replace('ios-v', '');

let gitTags = child_process.execSync('git tag --list ios-v*.*.*')
    .toString()
    .split('\n')
    .map(function (tag) {
        tag = tag.replace('ios-v', '').trim();
        return semver.clean(tag);
    });
let previousVersion = semver.maxSatisfying(gitTags, "<" + currentVersion);

/*
  Parse the raw changelog text and split it into individual releases.

  This regular expression:
    - Matches lines starting with "## ".
    - Groups the version number.
    - Skips the (optional) release date.
    - Groups the changelog content.
    - Ends when another "## " is found.
*/
const regex = /^## (\d+\.\d+\.\d+).*?\n(.+?)(?=\n^## )/gms;

let releaseNotes = [];
while (match = regex.exec(changelog)) {
    releaseNotes.push({
        'version': match[1],
        'changelog': match[2].trim(),
    });
}

/*
  Match the current tag with the most appropriate release notes.
*/
const versionsInReleaseNotes = _.map(releaseNotes, 'version');
const bestReleaseNotesForCurrentVersion = semver.minSatisfying(versionsInReleaseNotes, ">=" + currentVersion);
const currentReleaseNotes = _.find(releaseNotes, { version: bestReleaseNotesForCurrentVersion });

/*
  Fill and print the release notes template.
*/
const templatedReleaseNotes = ejs.render(fs.readFileSync('platform/ios/scripts/release-notes.md.ejs', 'utf8'), {
    'CURRENTVERSION': currentVersion,
    'PREVIOUSVERSION': previousVersion,
    'CHANGELOG': currentReleaseNotes.changelog,
    'isPrerelease': semver.prerelease(currentVersion)
});

process.stdout.write(templatedReleaseNotes);
