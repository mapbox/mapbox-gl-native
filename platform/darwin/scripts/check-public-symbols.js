#!/usr/bin/env node

'use strict';

const fs = require('fs');
const path = require('path');
const execFileSync = require('child_process').execFileSync;
const _ = require('lodash');

const keyword = /\bMGL_EXPORT\b/;

let scanned = [];

function hasMissingSymbols(os) {
  let missing = false;
  let sdk = os === 'iOS' ? 'iphonesimulator' : 'macosx';
  let sysroot = execFileSync('xcrun', ['--show-sdk-path', '--sdk', sdk]).toString().trim();
  let umbrellaPath = `platform/${os.toLowerCase()}/src/Mapbox.h`;
  let docArgs = ['doc', '--objc', umbrellaPath, '--',
                 '-x', 'objective-c', '-I', 'platform/darwin/src/', '-isysroot', sysroot];
  let docStr = execFileSync('sourcekitten', docArgs, { maxBuffer: Infinity }).toString().trim();
  let docJson = JSON.parse(docStr);
  _.forEach(docJson, function (result) {
    _.forEach(result, function (structure, path) {
      // Prevent multiple scans of the same file.
      if (scanned.indexOf(path) >= 0) return;
      scanned.push(path);

      const src = fs.readFileSync(path, 'utf8').split('\n');
      _.forEach(structure['key.substructure'], function (substructure) {
        switch (substructure['key.kind']) {
          case 'sourcekitten.source.lang.objc.decl.class':
            if (!(keyword.test(src[substructure['key.doc.line'] - 1]) || keyword.test(src[substructure['key.doc.line'] - 2]))) {
              console.warn(`- missing symbol export for class ${substructure['key.name']} in ${path}:${substructure['key.doc.line']}:${substructure['key.doc.column']}`);
              missing = true;
            }
            break;
          case 'sourcekitten.source.lang.objc.decl.constant':
            if (!keyword.test(src[substructure['key.doc.line'] - 1])) {
              console.warn(`- missing symbol export for constant ${substructure['key.name']} in ${path}:${substructure['key.doc.line']}:${substructure['key.doc.column']}`);
              missing = true;
            }
            break;
        }
      });
    });
  });

  return missing;
}

function ensureSourceKittenIsInstalled() {
  try {
    execFileSync('which', ['sourcekitten']);
  } catch (e) {
    console.log(`Installing SourceKitten via Homebrew…`);
    execFileSync('brew', ['install', 'sourcekitten']);
  }
}

if (process.argv.length < 3) {
  console.warn(`Usage: ${path.relative(process.cwd(), process.argv[1])} [macOS|iOS] ...`);
  process.exit(1);
}

ensureSourceKittenIsInstalled();

let missing = false;
for (var i = 2; i < process.argv.length; i++) {
  let os = process.argv[i];
  if (os == 'iOS' || os == 'macOS') {
    missing |= hasMissingSymbols(os);
  } else {
    console.warn(`Argument must be one of iOS or macOS`);
    process.exit(1);
  }
}

if (missing) {
  process.exit(1);
} else {
  console.warn(`All symbols are correctly exported.`);
}
