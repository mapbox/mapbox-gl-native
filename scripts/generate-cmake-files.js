#!/usr/bin/env node

const child_process = require('child_process');
const fs = require('fs');
const ejs = require('ejs');
const path = require('path');
const xcode = require('xcode');

require('./style-code');

const fileListTxt = ejs.compile(fs.readFileSync('cmake/files.txt.ejs', 'utf8'), {strict: true});

function generateFileList(filename, root, regex, patterns) {
    const files = child_process.execSync(`git -C ${root} ls-files ${patterns.map((p) => '"' + p + '"').join(' ')}`).toString().trim().split('\n');
    var groups = {};
    for (const file of files) {
        const match = file.match(regex);
        const group = match[1] || path.basename(filename, '-files.txt')
        if (!groups[group]) {
            groups[group] = [];
        }
        groups[group].push(file);
    }

    writeIfModified(filename, fileListTxt({ groups: groups }));
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

generateFileList('cmake/core-files.txt', '.', /^(?:src|include)\/(?:mbgl\/)?(.+)\/[^\/]+$/,
    [ 'include/*.hpp', 'include/*.h', 'src/*.hpp', 'src/*.cpp', 'src/*.h', 'src/*.c' ]);

generateFileList('cmake/benchmark-files.txt', '.', /^benchmark\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'benchmark/*.hpp', 'benchmark/*.cpp', 'benchmark/*.h', 'benchmark/*.c' ]);

generateFileList('cmake/test-files.txt', '.', /^test\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'test/*.hpp', 'test/*.cpp', 'test/*.h', 'test/*.c' ]);

generateXcodeSourceList('platform/macos/macos.xcodeproj', 'dynamic', 'sdk');

generateXcodeSourceList('platform/ios/ios.xcodeproj', 'dynamic', 'sdk');

const vendorRegex = /^(?:(?:src|include)\/)?(?:(.+)\/)?[^\/]+$/
generateFileList('vendor/boost-files.txt', 'vendor/boost', vendorRegex, [ "include/**/*.hpp", "include/**/*.h" ]);
generateFileList('vendor/cheap-ruler-cpp-files.txt', 'vendor/cheap-ruler-cpp', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/earcut.hpp-files.txt', 'vendor/earcut.hpp', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/eternal-files.txt', 'vendor/eternal', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/expected-files.txt', 'vendor/expected', vendorRegex, [ "include/expected.hpp" ]);
generateFileList('vendor/geojson-vt-cpp-files.txt', 'vendor/geojson-vt-cpp', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/geojson.hpp-files.txt', 'vendor/geojson.hpp', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/geometry.hpp-files.txt', 'vendor/geometry.hpp', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/icu-files.txt', 'vendor/icu', vendorRegex, [ "include/**/*.h", "src/*.h", "src/*.cpp" ]);
generateFileList('vendor/jni.hpp-files.txt', 'vendor/jni.hpp', vendorRegex, [ "include/**/*.hpp", ":!:include/jni/string_conversion.hpp" ]);
generateFileList('vendor/kdbush.hpp-files.txt', 'vendor/kdbush.hpp', vendorRegex, [ "include/*.hpp" ]);
generateFileList('vendor/pixelmatch-cpp-files.txt', 'vendor/pixelmatch-cpp', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/polylabel-files.txt', 'vendor/polylabel', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/protozero-files.txt', 'vendor/protozero', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/rapidjson-files.txt', 'vendor/rapidjson', vendorRegex, [ "include/**/*.h" ]);
generateFileList('vendor/shelf-pack-cpp-files.txt', 'vendor/shelf-pack-cpp', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/sqlite-files.txt', 'vendor/sqlite', vendorRegex, [ "include/*.h", "src/*.c" ]);
generateFileList('vendor/supercluster.hpp-files.txt', 'vendor/supercluster.hpp', vendorRegex, [ "include/*.hpp" ]);
generateFileList('vendor/unique_resource-files.txt', 'vendor/unique_resource', vendorRegex, [ "unique_resource.hpp" ]);
generateFileList('vendor/variant-files.txt', 'vendor/variant', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/vector-tile-files.txt', 'vendor/vector-tile', vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/wagyu-files.txt', 'vendor/wagyu', vendorRegex, [ "include/**/*.hpp" ]);
