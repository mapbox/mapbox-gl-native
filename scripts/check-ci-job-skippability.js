#!/usr/bin/env node

/*
This script takes two parameters or three environment variables:

  {CI job name|$CIRCLE_JOB} {SHA1...SHA1|$CIRCLE_MERGE_BASE...$CIRCLE_SHA1}

The flow of this script:

  - List of files changed in PR: `git diff --name-only start...end`
  - Extract list into array.
  - Remove items from array if they match:
    - Global ignore list.
    - Other platform ignore lists.
  - If array still contains items, do not authorize skipping CI job.
  - Else if array is empty, skip CI job.

To add support for a new platform:

  - Add a new PLATFORM_FILES array with a list of unique paths for the
    files belonging to the platform.
  - Add a case to `skippableFilePatternsForCIJob()` with:
    - The prefix for the platform's CI job names.
    - A concatenated array of other platform paths that are safe to ignore
      for the new platform. These should be files that can change without
      having any effect on the output of the new platform.
  - Update other cases in `skippableFilePatternsForCIJob()` to include the
    paths for the new platform.
*/

const execSync = require('child_process').execSync;
const _ = require('lodash');
const minimatch = require("minimatch");

console.step = _.partial(console.log, '\n\033[1m\033[36m*', _, '\033[0m');

// BUILD PARAMETERS
const CI_JOB_NAME = process.argv[2] || process.env.CIRCLE_JOB;
if (!CI_JOB_NAME) { console.error('You must specify the name of a CI job as a parameter or environment variable.'); process.exit(1); }

const COMMIT_RANGE = process.argv[3] || `${process.env.CIRCLE_MERGE_BASE}...${process.env.CIRCLE_SHA1}`;
if (!COMMIT_RANGE || COMMIT_RANGE.includes('undefined')) { console.error('You must specify a range of commits to check for changed files in as a parameter or environment variable.'); process.exit(1); }

// FILE SKIPPING PATTERNS
const ALWAYS_SKIPPABLE_FILES = [
  '**/CHANGELOG.md',
  '**/README.md',
  '**/CONTRIBUTING.md',
  '**/DEVELOPING.md',
  '**/INSTALL.md',
  '**/*.podspec'
]

const ANDROID_FILES = [
  'platform/android/**'
]

const DARWIN_FILES = [
  'platform/darwin/**'
]

const IOS_FILES = [
  'platform/ios/**'
]

const MACOS_FILES = [
  'platform/macos/**'
]

const OTHER_PLATFORMS = [
  'platform/glfw/**',
  'platform/linux/**',
  'platform/node/**',
  'platform/qt/**',
  'benchmark/**',
  'test/**'
]

function skippableFilePatternsForCIJob(job) {
  if (job.includes('render-tests')) {
      // Render tests run on specific platforms are effectively unskippable core jobs, not platform jobs.
      return ALWAYS_SKIPPABLE_FILES;
  } else if (job.startsWith('ios')) {
      return _.concat(ALWAYS_SKIPPABLE_FILES, ANDROID_FILES, MACOS_FILES, OTHER_PLATFORMS);
  } else if (job.startsWith('android')) {
      return _.concat(ALWAYS_SKIPPABLE_FILES, DARWIN_FILES, IOS_FILES, MACOS_FILES, OTHER_PLATFORMS);
  } else if (job.startsWith('macos')) {
      return _.concat(ALWAYS_SKIPPABLE_FILES, ANDROID_FILES, IOS_FILES, OTHER_PLATFORMS);
  } else {
      return ALWAYS_SKIPPABLE_FILES;
  }
}

console.step(`Getting list of files changed between ${COMMIT_RANGE}`);

let changedFiles = execSync(`git diff --name-only ${COMMIT_RANGE}`).toString().split('\n');
changedFiles = _.compact(changedFiles);

console.log(`${changedFiles.length} file${changedFiles.length == 1 ? '':'s'} changed.`);

console.step(`Checking changed files for paths relevant to ${CI_JOB_NAME} job`);

// Filter the changed files array to remove files that are irrelevant to the specified CI job.
_.remove(changedFiles, function(changedFile) {
  for (const skippableFilePattern of skippableFilePatternsForCIJob(CI_JOB_NAME)) {
    if (minimatch(changedFile, skippableFilePattern, { dot: true, nocase: true })) {
      return true;
    }
  }
  return false;
});

if (changedFiles.length) {
  console.log(`Found ${changedFiles.length} unskippable changed file${changedFiles.length == 1 ? '':'s'}:\n`, changedFiles);
} else {
  console.log(`Found no relevant changed files, so it's safe to skip the remainder of this CI job.`)
  if (process.env.CIRCLECI) {
    console.step('Aborting CI job');
    execSync(`circleci step halt`, {stdio: 'inherit'});
  }
}
