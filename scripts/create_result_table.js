#!/usr/bin/env node

var fs = require('fs');
var path = require('path');

var html =
    '<style>body { font-family: Helvetica; }</style>\n' +
    '<table>\n' +
    '<tr>\n' +
    '    <th>Actual</th>\n' +
    '    <th>Expected</th>\n' +
    '    <th>Info</th>\n' +
    '</tr>\n';

var dir = fs.readdirSync('./test/fixtures/styles').filter(function(name) {
    return name.match(/\.info\.json$/);
}).forEach(function(name) {
    var info = require(path.join(path.resolve('.'), './test/fixtures/styles', name));
    var base = path.basename(name, '.info.json');

    html +=
        '<tr>\n' +
        '    <td><img src="' + base + '.actual.png"></td>\n' +
        '    <td><img src="https://raw.githubusercontent.com/mapbox/mapbox-gl-native/' + process.env.TRAVIS_COMMIT + '/test/fixtures/styles/' + base + '.expected.png"></td>\n' +
        '    <td>\n' +
        '        <h2 style="text-align:center"><a href="https://raw.githubusercontent.com/mapbox/mapbox-gl-native/' + process.env.TRAVIS_COMMIT + '/test/fixtures/styles/' + base + '.style.json">' + base + '</a></h2>\n' +
        '        <ul>\n' +
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
});

html += "</table\n";

fs.writeFileSync('./test/fixtures/styles/index.html', html);
