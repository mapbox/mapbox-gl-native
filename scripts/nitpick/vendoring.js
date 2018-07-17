#!/usr/bin/env node
const nitpick = require('.');
const fs = require('fs');
const {execSync} = require('child_process');

const head = process.env['CIRCLE_SHA1'];
const mergeBase = process.env['CIRCLE_MERGE_BASE'];
if (!mergeBase) {
    console.log('No merge base available.');
    return;
}

const checkAll = process.argv.indexOf('--check-all') >= 2;

// Run the vendoring script for all vendored packages that were modified in this PR, and check for changes.
fs.readdirSync('vendor')
  .filter(name => name[0] != '.')
  .filter(name => fs.statSync(`vendor/${name}`).isDirectory())
  .filter(name => checkAll || execSync(`git diff --shortstat ${mergeBase} ${head} -- vendor/${name}`).toString().trim())
  .forEach(name => {
    execSync(`scripts/vendor/${name}.sh`);

    // List missing files
    var missing = execSync(`git ls-files --others --exclude-standard -- vendor/${name}`).toString().trim();
    if (!missing.length) {
      nitpick.ok(`All files vendored for ${name} are checked in`);
    } else {
      nitpick.fail(`These vendored files for ${name} are not checked in:`, missing);
    }

    // Diff existing files
    const diff = execSync(`git -c color.ui=always diff -- vendor/${name}`).toString().trim();
    if (!diff.length) {
      nitpick.ok(`All files vendored for ${name} are unmodified`);
    } else {
      nitpick.fail(`These vendored files for ${name} have modifications:`, diff);
    }
  });
