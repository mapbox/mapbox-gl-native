#!/usr/bin/env node

const child_process = require('child_process');
const fs = require('fs');
const ejs = require('ejs');
const path = require('path');
const xcode = require('xcode');

require('./style-code');

const fileListTxt = ejs.compile(fs.readFileSync('cmake/files.txt.ejs', 'utf8'), {strict: true});

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

    writeIfModified(`cmake/${name}-files.txt`, fileListTxt({ groups: groups }));
}

function generateXcodeSourceList(project, target, name) {
    const projectPath = path.dirname(project);
    let objects = xcode.project(path.join(project, 'project.pbxproj')).parseSync().hash.project.objects;

    // Build reverse lookup table for file => group
    let objToGroup = {};
    Object.keys(objects.PBXGroup)
        .filter(name => !/_comment$/.test(name))
        .forEach(name => objects.PBXGroup[name].children
            .forEach(child => objToGroup[child.value] = name));

    // Retrieve the target
    const targetObj = Object.keys(objects.PBXNativeTarget)
        .filter(name => !/_comment$/.test(name))
        .map(name => objects.PBXNativeTarget[name])
        .filter(project => project.name == target)[0];

    // Retrieve the sources associated with the target.
    const sourcesObjs = targetObj.buildPhases
        .map(phase => objects.PBXSourcesBuildPhase[phase.value] || objects.PBXHeadersBuildPhase[phase.value])
        .filter(phase => phase);

    let groups = {};
    sourcesObjs
        .forEach(sourcesObj => sourcesObj.files
        .map(file => objects.PBXBuildFile[file.value].fileRef)
        .forEach(fileRef => {
            // Look up the full path, and fully qualified group name for every source reference
            let fileObj = objects.PBXFileReference[fileRef];

            // Ascend the group tree to find the full path.
            let fullPath = [ fileObj.path.replace(/^"(.+)"$/, '$1') ];
            for (let ref = fileRef, obj = fileObj; obj.sourceTree != 'SOURCE_ROOT' && ref in objToGroup;) {
                ref = objToGroup[ref];
                obj = objects.PBXGroup[ref];
                if (obj.path) {
                    fullPath.unshift(obj.path.replace(/^"(.+)"$/, '$1'));
                }
            }

            // Generate a group name from the Xcode source groups
            let groupName = [];
            for (let ref = fileRef, obj = fileObj; ref in objToGroup;) {
                ref = objToGroup[ref];
                obj = objects.PBXGroup[ref];
                if (obj.name) {
                    groupName.unshift(obj.name.replace(/^"(.+)"$/, '$1'));
                }
            }

            groupName = groupName.join('/');
            if (!groups[groupName]) {
                groups[groupName] = [];
            }
            fullPath.unshift(projectPath);
            groups[groupName].push(path.join.apply(path, fullPath));
        })
    );

    writeIfModified(`${projectPath}/${name || target}-files.txt`, fileListTxt({ groups: groups }));
}

generateFileList('core', /^(?:src|include)\/(?:mbgl\/)?(.+)\/[^\/]+$/,
    [ 'include/*.hpp', 'include/*.h', 'src/*.hpp', 'src/*.cpp', 'src/*.h', 'src/*.c' ]);

generateFileList('benchmark', /^benchmark\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'benchmark/*.hpp', 'benchmark/*.cpp', 'benchmark/*.h', 'benchmark/*.c' ]);

generateFileList('test', /^test\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'test/*.hpp', 'test/*.cpp', 'test/*.h', 'test/*.c' ]);

generateXcodeSourceList('platform/macos/macos.xcodeproj', 'dynamic', 'core');

generateXcodeSourceList('platform/ios/ios.xcodeproj', 'dynamic', 'core');
