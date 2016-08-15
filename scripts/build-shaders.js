#!/usr/bin/env node

var path = require('path');
var fs = require('fs');
var mkdirp = require('mkdirp');

var input_file = process.argv[2]
var output_file = process.argv[3];

if (!input_file || !output_file) {
    console.warn('No input or output file given.');
    console.warn('Usage: %s [input.vertex.glsl] [output.vertex.hpp]', path.basename(process.argv[1]));
    process.exit(1);
}

var components = path.basename(input_file).split('.');

var shader_name = components[0];
var shader_type = components[1];
var extension = components[2];

var data = fs.readFileSync(input_file, 'utf8');

// Replace uniform pragmas

var pragma_mapbox_regex = /(\s*)#pragma\s+mapbox\s*:\s+(define|initialize)\s+(low|medium|high)p\s+(float|vec(?:2|3|4))\s+(.*)/;

var data = data.split('\n').map(function(line) {
    var params = line.match(pragma_mapbox_regex);
    if (params) {
        if (params[2] === 'define') {
            return params[1] + 'uniform ' + params[3] + 'p ' + params[4] + ' u_' + params[5] + ';';
        } else {
            return params[1] + params[3] + 'p ' + params[4] + ' ' + params[5] + ' = u_' + params[5] + ';';
        }
    } else {
        return line;
    }
}).join('\n');


var content =
    '#pragma once\n' +
    '\n' +
    '// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.\n' +
    '\n' +
    '#include <mbgl/gl/gl.hpp>\n' +
    '\n' +
    'namespace mbgl {\n' +
    'namespace shaders {\n' +
    'namespace ' + shader_name + ' {\n' +
    '\n' +
    '#ifndef MBGL_SHADER_NAME_' + shader_name.toUpperCase() + '\n' +
    '#define MBGL_SHADER_NAME_' + shader_name.toUpperCase() + '\n' +
    'constexpr const char* name = "' + shader_name + '";\n' +
    '#endif // MBGL_SHADER_NAME_' + shader_name.toUpperCase() + '\n' +
    '\n' +
    'constexpr const char* ' + shader_type + ' =\n' +
    '#ifdef GL_ES_VERSION_2_0\n' +
    '    "precision highp float;"\n' +
    '#else\n' +
    '    "#version 120"\n' +
    '#endif\n' +
    '    R"MBGL_SHADER(\n' + data + ')MBGL_SHADER";\n' +
    '\n' +
    '} // namespace ' + shader_name + '\n' +
    '} // namespace shaders\n' +
    '} // namespace mbgl\n';

mkdirp.sync(path.dirname(output_file));
fs.writeFileSync(output_file, content);
