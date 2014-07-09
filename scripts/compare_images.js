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

processFiles();

function processFiles() {
    if (!files.length) return done();

    var name = files.shift();

    var info = require(path.join(base_dir, name));
    var base = path.basename(name, '.info.json');

    var actual = path.join(base_dir, base + '.actual.png');
    var expected = path.join(base_dir, base + '.expected.png');
    var diff = path.join(base_dir, base + '.diff.png');

    var compare = spawn('compare', ['-metric', 'MAE', actual, expected, diff ]);
    var error = '';
    compare.stderr.on('data', function(data) {
        error += data.toString();
    });
    compare.on('exit', function(code, signal) {
        // The compare program returns 2 on error otherwise 0 if the images are similar or 1 if they are dissimilar.
        if (code == 2) {
            writeResult(base, info, error.trim(), Infinity);
            exitCode = 2;
        } else {
            var difference = parseFloat(error.match(/^\d+(?:\.\d+)?\s+\(([^\)]+)\)\s*$/)[1]);
            writeResult(base, info, '', difference);

        }
        return processFiles();
    });
    compare.stdin.end();
}

function writeResult(base, info, error, difference) {
    var color = 'green';
    if (difference > 0.01 && exitCode < 1) {
        exitCode = 1;
        color = 'red';
    }

    html +=
        '<tr>\n' +
        '    <td><img src="' + base + '.actual.png" onmouseover="this.src=\'' + base + '.expected.png\'" onmouseout="this.src=\'' + base + '.actual.png\'"></td>\n' +
        '    <td><img src="' + base + '.expected.png" onmouseover="this.src=\'' + base + '.diff.png\'" onmouseout="this.src=\'' + base + '.expected.png\'"></td>\n' +
        '    <td>\n' +
        '        <h2 style="text-align:center; background:' + color + '"><a href="' + base + '.style.json">' + base + '</a></h2>\n' +
        (error ? '        <p>' + error + '</p>\n' : '') +
        '        <ul>\n' +
        '            <li>diff: <strong>' + difference + '</strong></li>\n' +
        '            <li>zoom: <strong>' + (info.zoom || 0) + '</strong></li>\n' +
        '            <li>latitude: <strong>' + (info.latitude || 0) + '</strong></li>\n' +
        '            <li>longitude: <strong>' + (info.longitude || 0) + '</strong></li>\n' +
        '            <li>angle: <strong>' + (info.angle || 0) + '</strong></li>\n' +
        '            <li>width: <strong>' + (info.width || 0) + '</strong></li>\n' +
        '            <li>height: <strong>' + (info.height || 0) + '</strong></li>\n' +
        '        </ul>\n' +
        '    </td>\n' +
        '</tr>\n'
    ;
}

function done() {
    html += "</table>\n";

    fs.writeFileSync(path.join(base_dir, 'index.html'), html);

    process.exit(exitCode);
}