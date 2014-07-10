#!/usr/bin/env node

var fs = require('fs');
var path = require('path');
var spawn = require('child_process').spawn;

var base_dir = path.join(path.resolve('.'), 'test/fixtures/styles');

var files = fs.readdirSync(base_dir).filter(function(name) {
    return name.match(/\.info\.json$/);
});

var html =
    '<style>\n' +
    '    body { font-family: Helvetica; }\n' +
    '    h2 a { color:white; text-decoration:none; }\n' +
    '</style>\n' +
    '<table>\n' +
    '<tr>\n' +
    '    <th>Actual</th>\n' +
    '    <th>Expected</th>\n' +
    '    <th>Info</th>\n' +
    '</tr>\n';

var exitCode = 0;
var failures = 0;

processFiles();

function processFiles() {
    if (!files.length) return done();

    var name = files.shift();

    var info = require(path.join(base_dir, name));

    var keys = Object.keys(info);

    processFileTest();

    function processFileTest() {
        if (!keys.length) return processFiles();

        var key = keys.shift();

        var base = path.basename(name, '.info.json');

        var actual = path.join(base_dir, base + '/' + key + '.actual.png');
        var expected = path.join(base_dir, base + '/' + key + '.expected.png');
        var diff = path.join(base_dir, base + '/' + key + '.diff.png');

        var compare = spawn('compare', ['-metric', 'MAE', actual, expected, diff ]);
        var error = '';
        compare.stderr.on('data', function(data) {
            error += data.toString();
        });
        compare.on('exit', function(code, signal) {
            // The compare program returns 2 on error otherwise 0 if the images are similar or 1 if they are dissimilar.
            if (code == 2) {
                writeResult(base, key, info[key], error.trim(), Infinity);
                exitCode = 2;
            } else {
                var match = error.match(/^\d+(?:\.\d+)?\s+\(([^\)]+)\)\s*$/);
                var difference = match ? parseFloat(match[1]) : Infinity;
                writeResult(base, key, info[key], match ? '' : error, difference);

            }
            processFileTest();
        });
        compare.stdin.end();
    }
}

function writeResult(base, key, info, error, difference) {
    var color = 'green';
    var allowedDifference = ('diff' in info) ? info.diff : 0.01;
    if (difference > allowedDifference) {
        color = 'red';
        if (exitCode < 1) {
            exitCode = 1;
        }
        failures++;
    }

    html +=
        '<tr>\n' +
        '    <td><img src="' + base + '/' + key + '.actual.png" onmouseover="this.src=\'' + base + '/' + key + '.expected.png\'" onmouseout="this.src=\'' + base + '/' + key + '.actual.png\'"></td>\n' +
        '    <td><img src="' + base + '/' + key + '.expected.png" onmouseover="this.src=\'' + base + '/' + key + '.diff.png\'" onmouseout="this.src=\'' + base + '/' + key + '.expected.png\'"></td>\n' +
        '    <td>\n' +
        '        <h2 style="text-align:center; background:' + color + '"><a href="' + base + '.style.json">' + base + '/' + key + '</a></h2>\n' +
        (error ? '        <p>' + error + '</p>\n' : '') +
        '        <ul>\n' +
        '            <li>diff: <strong>' + difference + '</strong></li>\n' +
        '            <li>zoom: <strong>' + (info.zoom || 0) + '</strong></li>\n' +
        '            <li>center: <strong>' + (info.center || [0, 0]) + '</strong></li>\n' +
        '            <li>bearing: <strong>' + (info.bearing || 0) + '</strong></li>\n' +
        '            <li>width: <strong>' + (info.width || 512) + '</strong></li>\n' +
        '            <li>height: <strong>' + (info.height || 512) + '</strong></li>\n' +
        '        </ul>\n' +
        '    </td>\n' +
        '</tr>\n'
    ;
}

function done() {
    html += "</table>\n";

    fs.writeFileSync(path.join(base_dir, 'index.html'), html);
    console.warn('Results at: ' + path.join(base_dir, 'index.html'));
    if (failures) {
        console.warn('\x1B[1m\x1B[31m' + failures + ' ' + (failures == 1 ? 'image doesn\'t' : 'images don\'t') + ' match\x1B[39m\x1B[22m');
    } else {
        console.warn('\x1B[1m\x1B[32mAll images match\x1B[39m\x1B[22m');
    }

    process.exit(exitCode);
}