#!/usr/bin/env node

const child_process = require('child_process');
const fs = require('fs');
const ejs = require('ejs');

require('./style-code');

function generateCMakeListFile(name, regex, patterns) {
    const files = child_process.execSync(`git ls-files ${patterns.map((p) => '"' + p + '"').join(' ')}`).toString().trim().split('\n');
    var groups = {};
    for (const file of files) {
        const match = file.match(regex);
        const group = match[1] || name;
        if (!groups[group]) {
            groups[group] = [];
        }
        groups[group].push(file);
    }

    const fileListCmake = ejs.compile(fs.readFileSync('cmake/files.cmake.ejs', 'utf8'), {strict: true});
    writeIfModified(`cmake/${name}-files.cmake`, fileListCmake({ name: name, groups: groups }));
}

generateCMakeListFile('core', /^(?:src|include)\/(?:mbgl\/)?(.+)\/[^\/]+$/,
    [ 'include/*.hpp', 'include/*.h', 'src/*.hpp', 'src/*.cpp', 'src/*.h', 'src/*.c' ]);

generateCMakeListFile('benchmark', /^benchmark\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'benchmark/*.hpp', 'benchmark/*.cpp', 'benchmark/*.h', 'benchmark/*.c' ]);

generateCMakeListFile('test', /^test\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'test/*.hpp', 'test/*.cpp', 'test/*.h', 'test/*.c' ]);
