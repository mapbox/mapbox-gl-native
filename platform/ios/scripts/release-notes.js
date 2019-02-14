#!/usr/bin/env node

const fs = require('fs');
const execSync = require('child_process').execSync;
const ejs = require('ejs');
const _ = require('lodash');
const semver = require('semver');

const changelogPath = 'platform/ios/CHANGELOG.md';
const changelog = fs.readFileSync(changelogPath, 'utf8');

let outputMode = {};
switch(process.argv[2]) {
    case "jazzy":
        outputMode.isJazzy = true; break;
    case "github":
    default:
        outputMode.isGitHub = true;
}

/*
  Find current and immediately previous releases by parsing git tags.
*/
let currentVersion = execSync('git describe --tags --match=ios-v*.*.* --abbrev=0')
    .toString()
    .trim()
    .replace('ios-v', '');

let gitTags = execSync('git tag --list ios-v*.*.*')
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
    - Matches lines starting with "## x.x.x".
    - Groups the version number.
    - Skips the (optional) release date.
    - Groups the changelog content.
    - Ends when another "## x.x.x" is found.
*/
const regex = /^## (\d+\.\d+\.\d+).*?\n(.+?)(?=\n^## \d+\.\d+\.\d+.*?\n)/gms;

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

if (!currentReleaseNotes) {
    console.error('Could not find a release section satisfying %s in %s — did you forget to rename the "master" section to %s?', currentVersion, changelogPath, currentVersion.split("-")[0]);
    process.exit(1);
}

/*
  Fill and print the release notes template.
*/
let templatedReleaseNotes;

if (outputMode.isGitHub) {
    templatedReleaseNotes = ejs.render(fs.readFileSync('platform/ios/scripts/release-notes-github.md.ejs', 'utf8'), {
        'CURRENTVERSION': currentVersion,
        'PREVIOUSVERSION': previousVersion,
        'CHANGELOG': currentReleaseNotes.changelog,
        'isPrerelease': semver.prerelease(currentVersion)
    });
}

if (outputMode.isJazzy) {
    const minorReleaseSeries = semver.major(currentVersion) + "." + semver.minor(currentVersion) + ".0";
    const range = ">=" + minorReleaseSeries + " <" + currentVersion;
    const otherReleasesInSeries = _.filter(releaseNotes, function(release) {
        return semver.satisfies(release.version, range);
    });

    otherReleasesInSeries.forEach(function(release) {
        // Bump section headings from h3 to h4.
        release.changelog = release.changelog.replace(/^### /gm, '#### ');
    });

    templatedReleaseNotes = ejs.render(fs.readFileSync('platform/ios/scripts/release-notes-jazzy.md.ejs', 'utf8'), {
        'CURRENTVERSION': currentVersion,
        'CURRENTCHANGELOG': currentReleaseNotes.changelog,
        'OTHERRELEASES': otherReleasesInSeries
    });
}

process.stdout.write(templatedReleaseNotes);
