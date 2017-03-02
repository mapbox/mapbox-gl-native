#!/usr/bin/env node

const path = require('path');
const fs = require('fs');

const inputPath = 'mapbox-gl-js/src/shaders';
const outputPath = 'src/mbgl/shaders';

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
            const a_type = type === "float" ? "vec2" : "vec4";
            return pragmas[operation]
                .join("\n")
                .replace(/\{type\}/g, type)
                .replace(/\{a_type}/g, a_type)
                .replace(/\{precision\}/g, precision)
                .replace(/\{name\}/g, name);
        });
    }

    function unpackAttrs(){
        float

    }


    function vertexSource() {
        const prelude = fs.readFileSync(path.join(inputPath, '_prelude.vertex.glsl'));
        const source = fs.readFileSync(path.join(inputPath, shaderName + '.vertex.glsl'), 'utf8');
        return prelude + applyPragmas(source, {
                define: [
                    "uniform lowp float a_{name}_t;",
                    "attribute {precision} {a_type} a_{name}_minmax;",
                    "varying {precision} {type} {name};"
                ],
                initialize: [
                    "{name} = unpack_mix_{a_type}(a_{name}_minmax, a_{name}_t);"
                ]
            });
    }

    function fragmentSource() {
        const prelude = fs.readFileSync(path.join(inputPath, '_prelude.fragment.glsl'));
        const source = fs.readFileSync(path.join(inputPath, shaderName + '.fragment.glsl'), 'utf8');
        return prelude + applyPragmas(source, {
                define: [
                    "varying {precision} {type} {name};"
                ],
                initialize: [
                ]
            });
    }

    writeIfModified(path.join(outputPath, `${shaderName}.hpp`), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#pragma once

namespace mbgl {
namespace shaders {

class ${shaderName} {
public:
    static const char* name;
    static const char* vertexSource;
    static const char* fragmentSource;
};

} // namespace shaders
} // namespace mbgl
`);

    writeIfModified(path.join(outputPath, `${shaderName}.cpp`), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/${shaderName}.hpp>

namespace mbgl {
namespace shaders {

const char* ${shaderName}::name = "${shaderName}";
const char* ${shaderName}::vertexSource = R"MBGL_SHADER(
${vertexSource()}
)MBGL_SHADER";
const char* ${shaderName}::fragmentSource = R"MBGL_SHADER(
${fragmentSource()}
)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
`);
});
