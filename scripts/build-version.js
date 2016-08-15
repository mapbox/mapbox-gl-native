#!/usr/bin/env node

var path = require('path');
var fs = require('fs');
var util = require('util');
var mkdirp = require('mkdirp');
var execSync = require('child_process').execSync;

const DEFAULT_TAG = [0, 0, 0];
const DEFAULT_REV = 'unknown';

function is_git_repo() {
    try {
        execSync('git rev-parse', { stdio: ['ignore', 'ignore', 'ignore'] });
        return true;
    } catch(err) {
        return false;
    }
}


function parse_tag(raw_tag) {
    return raw_tag.replace(/[^0-9.]/g, '').split('.').map(parseFloat).slice(0, 3);
}

function parse_rev(raw_rev) {
    return raw_rev.substr(0, 8);
}

var output_dir = process.argv[2];

if (!output_dir) {
    console.warn('No output directory given.');
    console.warn('Usage: %s [output dir]', path.basename(process.argv[1]));
    process.exit(1);
}

if (is_git_repo()) {
    var raw_tag = execSync('git describe --tags --always --abbrev=0').toString().trim();
    var raw_rev = execSync('git rev-parse HEAD').toString().trim();

    // When they're identical, the "git describe" can't find a tag and reports the rev instead.
    if (raw_tag == raw_rev) {
        var tag = DEFAULT_TAG
        var rev = parse_rev(raw_rev)
    } else {
        var tag = parse_tag(raw_tag)
        var rev = parse_rev(raw_rev)
    }
}  else {
    var tag = DEFAULT_TAG;
    var rev = DEFAULT_REV;
}

console.log('Tag: %d.%d.%d', tag[0], tag[1], tag[2]);
console.log('Rev: %s', rev);

var header = '// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.\n' +
    '#pragma once\n' +
    '\n' +
    '#define MBGL_VERSION 0x' + (tag[0] << 16 | tag[1] << 8 | tag[2]).toString(16) + '\n' +
    '#define MBGL_VERSION_STRING "' + tag[0] + '.' + tag[1] + '.' + tag[2] + '"\n' +
    '#define MBGL_VERSION_MAJOR ' + tag[0] + '\n' +
    '#define MBGL_VERSION_MINOR ' + tag[1] + '\n' +
    '#define MBGL_VERSION_PATCH ' + tag[2] + '\n' +
    '#define MBGL_VERSION_REV "' + rev + '"\n' +
    '\n' +
    'namespace mbgl {\n' +
    'namespace version {\n' +
    '\n' +
    'extern const int major, minor, patch;\n' +
    'extern const char *revision;\n' +
    'extern const char *string;\n' +
    'extern const unsigned int number;\n' +
    '\n' +
    '} // namespace version\n' +
    '} // namespace mbgl\n';

var header_path = path.join(output_dir, 'include/mbgl/util/version.hpp')
mkdirp.sync(path.dirname(header_path));
fs.writeFileSync(header_path, header);
