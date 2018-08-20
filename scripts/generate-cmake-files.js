#!/usr/bin/env node

const child_process = require('child_process');
const fs = require('fs');
const ejs = require('ejs');

require('./style-code');

function generateFileList(name, regex, patterns) {
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

    const fileListCmake = ejs.compile(fs.readFileSync('cmake/files.txt.ejs', 'utf8'), {strict: true});
    writeIfModified(`cmake/${name}-files.txt`, fileListCmake({ groups: groups }));
}

generateFileList('core', /^(?:src|include)\/(?:mbgl\/)?(.+)\/[^\/]+$/,
    [ 'include/*.hpp', 'include/*.h', 'src/*.hpp', 'src/*.cpp', 'src/*.h', 'src/*.c' ]);

generateFileList('benchmark', /^benchmark\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'benchmark/*.hpp', 'benchmark/*.cpp', 'benchmark/*.h', 'benchmark/*.c' ]);

generateFileList('test', /^test\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'test/*.hpp', 'test/*.cpp', 'test/*.h', 'test/*.c' ]);
