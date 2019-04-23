import { writeIfModified } from '../definitions/globals.js';

let fs = require('fs');
let ejs = require('ejs');
let path = require('path');

var totalCount = 0;
var javaIgnoreCount = 0;
var nodeIgnoreCount = 0;
var relativePath = '';
var pathToFile = '';

export default function(api, type) {
    // Copy gl-js integration test folder to android assets
    copyFolderSync('../../../mapbox-gl-js/test/integration/', 'src/main/assets/', 'node_modules');

    // Walk through render-test directory and generate tests
    recursiveDirectoryWalk('../../../mapbox-gl-js/test/integration/render-tests/', type, api['java-ignores'], api['node-ignores']);

    // Print code gen result output
    console.log('\nFinished generating render tests for ' + type + ': \ngenerated: ' + totalCount + '\njava-ignored: ' + javaIgnoreCount+ '\nnode-ignored: ' + nodeIgnoreCount);
    return {};
}

function recursiveDirectoryWalk(dir, type, javaIgnores, nodeIgnores) {
    let testSubDir = "test/integration/render-tests/";
    const files = fs.readdirSync(dir);
    for (const file of files) {
        pathToFile = path.join(dir, file);
        const isDirectory = fs.statSync(pathToFile).isDirectory();
        if (isDirectory) {
            recursiveDirectoryWalk(pathToFile, type, javaIgnores, nodeIgnores);
        } else if (file.endsWith('style.json')) {
            relativePath = pathToFile.split(testSubDir)[1].slice(0, -11);
            let testPath = 'render-tests/' + relativePath;
            if (javaIgnores.hasOwnProperty(testPath)) {
                // This test is ignored due to the ignores files for android
                javaIgnoreCount++;
                continue;
            }

            if (nodeIgnores.hasOwnProperty(testPath)) {
                // This test is ignored due to the ignores files for node
                nodeIgnoreCount++;
                continue;
            }

            // TODO add ignore for unsupported operations and properties

            totalCount++;

            generateTestFiles(dir);
        }
    }
}

function generateTestFiles(dir) {
    // read style definition, optimise test metadata
    const style_json = JSON.parse(fs.readFileSync(pathToFile, 'utf8'));
    const test_metadata = createTestMetadata(style_json, pathToFile, relativePath);

    // generate test
    const template = ejs.compile(fs.readFileSync('templates/render-test.java.ejs', 'utf8'), {strict: true});
    writeIfModified('src/androidTest/java/com/mapbox/mapboxsdk/test/render/' + test_metadata['test']['package'] + '/'+ test_metadata['test']['name'] + '.java', template(test_metadata));
}

function createTestMetadata(style_json) {
    var test_metadata = style_json['metadata'];

    // ensure metadata availability
    if (!test_metadata) {
        test_metadata = {};
    }
    if (!test_metadata['test']) {
        test_metadata['test'] = {};
    }

    // optimize test data for java language conventions
    const pathElements = santizePathForJavaConventions(pathToFile).split('/');
    const javaPath = pathElements[pathElements.length - 3].replace(/-/g,'_');
    test_metadata['test']['package'] = javaPath;
    test_metadata['test']['asset_path'] = 'render-tests/' + relativePath;
    test_metadata['test']['result'] = relativePath;
    var testName = camelize(relativePath.split("/")[1]).replace("-","");
    test_metadata['test']['name'] = testName;
    return test_metadata;
}

function copyFolderSync(from, to, ignoreDir) {
    try {
      ensureDirSync(to)
    } catch (err) {
    }
    fs.readdirSync(from).forEach(element => {
        if (fs.lstatSync(path.join(from, element)).isFile()) {
            fs.copyFileSync(path.join(from, element), path.join(to, element));
        } else {
            if (!from.endsWith(ignoreDir)) {
              copyFolderSync(path.join(from, element), path.join(to, element), ignoreDir);
            }
        }
    });
}

function ensureDirSync (dirpath) {
  try {
    fs.mkdirSync(dirpath, { recursive: true })
  } catch (err) {
    if (err.code !== 'EEXIST') throw err
  }
}

global.camelize = function (str) {
  return str.replace(/(?:^|-)(.)/g, function (_, x) {
    return x.toUpperCase();
  });
}

global.santizePathForJavaConventions = function (str) {
  return str.replace(/\/$/, '')
    .replace('default','defaux')
    .replace('#',"_")
    .replace('@','at')
    .replace('false', 'faux');
}

