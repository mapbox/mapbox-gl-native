#!/usr/bin/env node

var path = require('path');
var fs = require('fs');

var inputPath = 'mapbox-gl-js/shaders';
var outputPath = 'src/mbgl/shaders';

require('./style-code');

[
    'circle',
    'collision_box',
    'debug',
    'fill',
    'fill_outline',
    'fill_outline_pattern',
    'fill_pattern',
    'line',
    'line_pattern',
    'line_sdf',
    'raster',
    'symbol_icon',
    'symbol_sdf'
].forEach(function (shaderName) {
    function applyPragmas(source, pragmas) {
        return source.replace(/#pragma mapbox: ([\w]+) ([\w]+) ([\w]+) ([\w]+)/g, (match, operation, precision, type, name) => {
            return pragmas[operation]
                .join("\n")
                .replace(/\{type\}/g, type)
                .replace(/\{precision\}/g, precision)
                .replace(/\{name\}/g, name);
        });
    }

    function vertexSource() {
        var prelude = fs.readFileSync(path.join(inputPath, '_prelude.vertex.glsl'));
        var source = fs.readFileSync(path.join(inputPath, shaderName + '.vertex.glsl'), 'utf8');
        return prelude + applyPragmas(source, {
                define: [
                    "uniform lowp float a_{name}_t;",
                    "attribute {precision} {type} a_{name}_min;",
                    "attribute {precision} {type} a_{name}_max;",
                    "varying {precision} {type} {name};"
                ],
                initialize: [
                    "{name} = mix(a_{name}_min, a_{name}_max, a_{name}_t);"
                ]
            });
    }

    function fragmentSource() {
        var prelude = fs.readFileSync(path.join(inputPath, '_prelude.fragment.glsl'));
        var source = fs.readFileSync(path.join(inputPath, shaderName + '.fragment.glsl'), 'utf8');
        return prelude + applyPragmas(source, {
                define: [
                    "varying {precision} {type} {name};"
                ],
                initialize: [
                ]
            });
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
    "class " + shaderName + " {\n" +
    "public:\n" +
    "    static constexpr const char* name = \"" + shaderName + "\";\n" +
    "    static constexpr const char* vertexSource = R\"MBGL_SHADER(\n" + vertexSource() + ")MBGL_SHADER\";\n" +
    "    static constexpr const char* fragmentSource = R\"MBGL_SHADER(\n" + fragmentSource() + ")MBGL_SHADER\";\n" +
    "};\n" +
    "\n" +
    "} // namespace shaders\n" +
    "} // namespace mbgl\n";

    writeIfModified(path.join(outputPath, shaderName + '.hpp'), content);
});
