#!/usr/bin/env node

const child_process = require('child_process');
const fs = require('fs');
const path = require('path');
const xcode = require('xcode');

require('./style-code');

const classifier = /^(?:(?:(?:platform|vendor)\/(?:(?!include|src).)*\/|(?:test|benchmark)\/)?(?:(include|src)\/)?)?(.+\.h(?:pp|xx)?)$/;

function generateFileList(filename, roots, regex, patterns) {
    writeFileList(
        filename,
        [].concat.apply([], roots.map(function(root) {
          return child_process
            .execSync(`git -C ${root} ls-files ${patterns.map((p) => '"' + p + '"').join(' ')}`)
            .toString()
            .trim()
            .split('\n')
            .map(file => path.join(root, file))
        })))
}

function writeFileList(filename, files) {
    const json = {
        "//": "This file is generated. Do not edit. Regenerate it with scripts/generate-file-lists.js",
        sources: [],
        public_headers: {},
        private_headers: {}
    };
    files.sort();
    for (const file of files) {
        const match = file.match(classifier);
        if (!match) {
            json.sources.push(file);
        } else if (match[1] === 'src') {
            json.private_headers[match[2]] = file;
        } else {
            if (match[1] !== 'include') {
                console.warn(`* ${file} is not in include or src directory. Treating as public header.`);
            }
            json.public_headers[match[2]] = file;
        }
    }
    writeIfModified(filename, JSON.stringify(json, null, 4) + '\n');
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

    const json = {
        "//": "This file is generated. Do not edit. Regenerate it with scripts/generate-file-lists.js",
        sources: [],
        public_headers: {},
        private_headers: {}
    };

    sourcesObjs
        .forEach(sourcesObj => sourcesObj.files
            .map(file => objects.PBXBuildFile[file.value])
            .forEach(buildFile => {
                // const isPublic = buildFile.settings && buildFile.settings.ATTRIBUTES && buildFile.settings.ATTRIBUTES.indexOf('Public') >= 0 || false;
                const fileRef = buildFile.fileRef;
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
                fullPath.unshift(projectPath);

                const fullPathName = path.join.apply(path, fullPath);

                if (sourcesObj.isa === 'PBXHeadersBuildPhase') {
                    const isPublic = buildFile.settings && buildFile.settings.ATTRIBUTES && buildFile.settings.ATTRIBUTES.indexOf('Public') >= 0 || false;
                    json[isPublic ? 'public_headers' : 'private_headers'][path.basename(fullPathName)] = fullPathName;
                } else {
                    json.sources.push(fullPathName);
                }
            })
    );

    json.public_headers['Mapbox/Mapbox.h'] = json.public_headers['Mapbox.h'];
    delete json.public_headers['Mapbox.h'];

    writeIfModified(`${projectPath}/${name || target}-files.json`, JSON.stringify(json, null, 4) + '\n');
}

generateFileList('src/core-files.json', [ '.' ], /^(?:src|include)\/(?:mbgl\/)?(.+)\/[^\/]+$/,
    [ 'include/*.hpp', 'include/*.h', 'src/*.hpp', 'src/*.cpp', 'src/*.h', 'src/*.c' ]);

generateFileList('benchmark/benchmark-files.json', [ '.' ], /^benchmark\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'benchmark/*.hpp', 'benchmark/*.cpp', 'benchmark/*.host', 'benchmark/*.c' ]);

generateFileList('test/test-files.json', [ '.' ], /^test\/(?:(?:src|include)\/)?(?:mbgl\/)?(?:(.+)\/)?[^\/]+$/,
    [ 'test/*.hpp', 'test/*.cpp', 'test/*.h', 'test/*.c' ]);

generateXcodeSourceList('platform/macos/macos.xcodeproj', 'dynamic', 'sdk');

generateXcodeSourceList('platform/ios/ios.xcodeproj', 'dynamic', 'sdk');

const vendorRegex = /^(?:(?:src|include)\/)?(?:(.+)\/)?[^\/]+$/
generateFileList('vendor/args-files.json', [ 'vendor/args' ], vendorRegex, [ "args.hxx" ]);
generateFileList('vendor/boost-files.json', [ 'vendor/boost' ], vendorRegex, [ "include/**/*.hpp", "include/**/*.h" ]);
generateFileList('vendor/cheap-ruler-cpp-files.json', [ 'vendor/cheap-ruler-cpp' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/earcut.hpp-files.json', [ 'vendor/earcut.hpp' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/eternal-files.json', [ 'vendor/eternal' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/expected-files.json', [ 'vendor/expected' ], vendorRegex, [ "include/expected.hpp" ]);
generateFileList('vendor/filesystem-files.json', [ 'vendor/filesystem' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/geojson-vt-cpp-files.json', [ 'vendor/geojson-vt-cpp' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/geojson.hpp-files.json', [ 'vendor/geojson.hpp' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/icu-files.json', [ 'vendor/icu' ], vendorRegex, [ "include/**/*.h", "src/*.h", "src/*.cpp" ]);
generateFileList('vendor/jni.hpp-files.json', [ 'vendor/jni.hpp' ], vendorRegex, [ "include/**/*.hpp", ":!:include/jni/string_conversion.hpp" ]);
generateFileList('vendor/kdbush.hpp-files.json', [ 'vendor/kdbush.hpp' ], vendorRegex, [ "include/*.hpp" ]);
generateFileList('vendor/mapbox-base-files.json', [ 'vendor/mapbox-base/libs/geometry.hpp', 'vendor/mapbox-base/libs/variant', 'vendor/mapbox-base/libs/optional' ], vendorRegex, [ "include/*.hpp", "include/**/*.hpp", "optional.hpp" ]);
generateFileList('vendor/pixelmatch-cpp-files.json', [ 'vendor/pixelmatch-cpp' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/polylabel-files.json', [ 'vendor/polylabel' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/protozero-files.json', [ 'vendor/protozero' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/rapidjson-files.json', [ 'vendor/rapidjson' ], vendorRegex, [ "include/**/*.h" ]);
generateFileList('vendor/shelf-pack-cpp-files.json', [ 'vendor/shelf-pack-cpp' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/sqlite-files.json', [ 'vendor/sqlite' ], vendorRegex, [ "include/*.h", "src/*.c" ]);
generateFileList('vendor/supercluster.hpp-files.json', [ 'vendor/supercluster.hpp' ], vendorRegex, [ "include/*.hpp" ]);
generateFileList('vendor/unique_resource-files.json', [ 'vendor/unique_resource' ], vendorRegex, [ "unique_resource.hpp" ]);
generateFileList('vendor/vector-tile-files.json', [ 'vendor/vector-tile' ], vendorRegex, [ "include/**/*.hpp" ]);
generateFileList('vendor/wagyu-files.json', [ 'vendor/wagyu' ], vendorRegex, [ "include/**/*.hpp" ]);
