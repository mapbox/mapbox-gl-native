#!/usr/bin/env node
const nitpick = require('.');
const child_process = require('child_process');
const path = require('path');
const fs = require('fs');
const os = require('os');

function checkGeneratedFiles(name, scripts) {
  var files = [];

  scripts.forEach(function(script) {
    child_process.execSync(script);
    const list = path.join(path.dirname(script), path.basename(script, path.extname(script)) + '.list');
    files.push(list);
    files = files.concat(fs.readFileSync(list, 'utf8').split('\n'));
  });

  // List missing files
  var missing = child_process.execFileSync('git', ['ls-files', '--others', '--exclude-standard', '--'].concat(files)).toString().trim();
  if (!missing.length) {
    nitpick.ok(`All generated ${name} files are checked in`);
  } else {
    nitpick.fail(`These generated ${name} files are not checked in:`, missing);
  }

  // Diff existing files
  var diff = child_process.execFileSync('git', ['-c', 'color.ui=always', 'diff', 'HEAD', '--'].concat(files)).toString().trim();
  if (!diff.length) {
    nitpick.ok(`All generated ${name} files are up-to-date`);
  } else {
    nitpick.fail(`These generated ${name} files have modifications:`, diff);
  }
}

const mode = (process.argv[2] || '').toLowerCase();
if (!mode || mode == 'cmake') {
  checkGeneratedFiles('CMake list', ['scripts/generate-cmake-files.js']);
}
if (!mode || mode == 'shader') {
  checkGeneratedFiles('shader', ['scripts/generate-shaders.js']);
}
if (!mode || mode == 'style') {
  checkGeneratedFiles('style', ['scripts/generate-style-code.js']);
}
if (!mode || mode == 'android') {
  checkGeneratedFiles('Android', ['platform/android/scripts/generate-style-code.js']);
}
if ((!mode || mode == 'darwin') && os.platform() == 'darwin') {
  checkGeneratedFiles('Darwin', ['platform/darwin/scripts/generate-style-code.js', 'platform/darwin/scripts/update-examples.js']);
}
