#!/usr/bin/env node

var path = require('path');
var fs = require('fs');
var mkdirp = require('mkdirp');

var shaderName = process.argv[2];
var inputPath = process.argv[3];
var outputPath = process.argv[4];

if (!shaderName || !inputPath || !outputPath) {
    console.warn('Not enough arguments.');
    console.warn('Usage: %s shaderName /path-to-shader-sources /output-path', path.basename(process.argv[1]));
    process.exit(1);
}

var pragmaMapboxRe = /(\s*)#pragma\s+mapbox\s*:\s+(define|initialize)\s+(low|medium|high)p\s+(float|vec(?:2|3|4))\s+(.*)/;


function applyPragmas(source) {
    return '\n' + source.split('\n').map(function(line) {
        var params = line.match(pragmaMapboxRe);
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

var vertexPrelude = fs.readFileSync(path.join(inputPath, '_prelude.vertex.glsl'));
var fragmentPrelude = fs.readFileSync(path.join(inputPath, '_prelude.fragment.glsl'));
var vertexSource = vertexPrelude + fs.readFileSync(path.join(inputPath, shaderName + '.vertex.glsl'), 'utf8');
var fragmentSource = fragmentPrelude + fs.readFileSync(path.join(inputPath, shaderName + '.fragment.glsl'), 'utf8');

var content = "#pragma once\n" +
"\n" +
"// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.\n" +
"\n" +
"#include <mbgl/gl/gl.hpp>\n" +
"\n" +
"namespace mbgl {\n" +
"namespace shaders {\n" +
"\n" +
"class " + shaderName + " {\n" +
"public:\n" +
"    static constexpr const char* name = \"" + shaderName + "\";\n" +
"    static constexpr const char* vertexSource = R\"MBGL_SHADER(" + applyPragmas(vertexSource) + ")MBGL_SHADER\";\n" +
"    static constexpr const char* fragmentSource = R\"MBGL_SHADER(" + applyPragmas(fragmentSource) + ")MBGL_SHADER\";\n" +
"};\n" +
"\n" +
"} // namespace shaders\n" +
"} // namespace mbgl\n";

mkdirp.sync(outputPath);
fs.writeFileSync(path.join(outputPath, shaderName + '.hpp'), content);
