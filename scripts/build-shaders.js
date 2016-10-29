#!/usr/bin/env node

var path = require('path');
var fs = require('fs');
var mkdirp = require('mkdirp');

var vertex_file = process.argv[2];
var fragment_file = process.argv[3];
var output_file = process.argv[4];

if (!vertex_file || !fragment_file || !output_file) {
    console.warn('No input or output file given.');
    console.warn('Usage: %s input.vertex.glsl input.fragment.glsl output.hpp', path.basename(process.argv[1]));
    process.exit(1);
}

var components = path.basename(vertex_file).split('.');
var shader_name = components[0];

var pragma_mapbox_regex = /(\s*)#pragma\s+mapbox\s*:\s+(define|initialize)\s+(low|medium|high)p\s+(float|vec(?:2|3|4))\s+(.*)/;

function source(file) {
    return '\n' + fs.readFileSync(file, 'utf8').split('\n').map(function(line) {
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
}

var content = "#pragma once\n" +
"\n" +
"// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.\n" +
"\n" +
"#include <mbgl/gl/gl.hpp>\n" +
"\n" +
"namespace mbgl {\n" +
"namespace shaders {\n" +
"\n" +
"class " + shader_name + " {\n" +
"public:\n" +
"    static constexpr const char* name = \"" + shader_name + "\";" +
"    static constexpr const char* vertexSource = R\"MBGL_SHADER(" + source(vertex_file) + ")MBGL_SHADER\";\n" +
"    static constexpr const char* fragmentSource = R\"MBGL_SHADER(" + source(fragment_file) + ")MBGL_SHADER\";\n" +
"};\n" +
"\n" +
"} // namespace shaders\n" +
"} // namespace mbgl\n";

mkdirp.sync(path.dirname(output_file));
fs.writeFileSync(output_file, content);
