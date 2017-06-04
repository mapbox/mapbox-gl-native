#!/usr/bin/env node

const path = require('path');
const fs = require('fs');

const inputPath = 'mapbox-gl-js/src/shaders';
const outputPath = 'src/mbgl/shaders';

require('./style-code');

const vertexPrelude = fs.readFileSync(path.join(inputPath, '_prelude.vertex.glsl'));
const fragmentPrelude = fs.readFileSync(path.join(inputPath, '_prelude.fragment.glsl'));

writeIfModified(path.join(outputPath, 'preludes.hpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#pragma once

namespace mbgl {
namespace shaders {

extern const char* vertexPrelude;
extern const char* fragmentPrelude;

} // namespace shaders
} // namespace mbgl
`);

writeIfModified(path.join(outputPath, 'preludes.cpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/preludes.hpp>

namespace mbgl {
namespace shaders {

const char* vertexPrelude = R"MBGL_SHADER(
${vertexPrelude}
)MBGL_SHADER";
const char* fragmentPrelude = R"MBGL_SHADER(
${fragmentPrelude}
)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
`);

[
    'circle',
    'collision_box',
    'debug',
    'extrusion_texture',
    'fill',
    'fill_extrusion',
    'fill_extrusion_pattern',
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
    const re = /#pragma mapbox: ([\w]+) ([\w]+) ([\w]+) ([\w]+)/g;
    const fragmentPragmas = new Set();

    let fragmentSource = fs.readFileSync(path.join(inputPath, shaderName + '.fragment.glsl'), 'utf8');
    let vertexSource = fs.readFileSync(path.join(inputPath, shaderName + '.vertex.glsl'), 'utf8');

    fragmentSource = fragmentSource.replace(re, (match, operation, precision, type, name) => {
        fragmentPragmas.add(name);
        return operation === "define" ? `
#ifndef HAS_UNIFORM_u_${name}
varying ${precision} ${type} ${name};
#else
uniform ${precision} ${type} u_${name};
#endif
` : `
#ifdef HAS_UNIFORM_u_${name}
    ${precision} ${type} ${name} = u_${name};
#endif
`;
    });

    vertexSource = vertexSource.replace(re, (match, operation, precision, type, name) => {
        const a_type = type === "float" ? "vec2" : "vec4";
        if (fragmentPragmas.has(name)) {
            return operation === "define" ? `
#ifdef HAS_UNIFORM_u_${name}
uniform lowp float a_${name}_t;
attribute ${precision} ${a_type} a_${name};
varying ${precision} ${type} ${name};
#else
uniform ${precision} ${type} u_${name};
#endif
` : `
#ifndef HAS_UNIFORM_u_${name}
    ${name} = unpack_mix_${a_type}(a_${name}, a_${name}_t);
#else
    ${precision} ${type} ${name} = u_${name};
#endif
`;
        } else {
            return operation === "define" ? `
#ifdef HAS_UNIFORM_u_${name}
uniform lowp float a_${name}_t;
attribute ${precision} ${a_type} a_${name};
#else
uniform ${precision} ${type} u_${name};
#endif
` : `
#ifndef HAS_UNIFORM_u_${name}
    ${precision} ${type} ${name} = unpack_mix_${a_type}(a_${name}, a_${name}_t);
#else
    ${precision} ${type} ${name} = u_${name};
#endif
`;
        }
    });

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
${vertexSource}
)MBGL_SHADER";
const char* ${shaderName}::fragmentSource = R"MBGL_SHADER(
${fragmentSource}
)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
`);
});
