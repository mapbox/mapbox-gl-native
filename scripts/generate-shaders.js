#!/usr/bin/env node

require('@mapbox/flow-remove-types/register');

const path = require('path');
const outputPath = 'src/mbgl/programs';
const crypto = require('crypto');

var shaders = require('../mapbox-gl-js/src/shaders');

require('./style-code');

let concatenated = '';
let offsets = {};

for (const key in shaders) {
    // Rename a_*_t uniforms to u_*_t. This is a workaround until we can use
    // https://github.com/mapbox/mapbox-gl-js/pull/8055, which is blocked by
    // https://github.com/mapbox/mapbox-gl-native/issues/13984
    shaders[key].vertexSource = shaders[key].vertexSource.replace(/\ba_(\w+)_t\b/mg, 'u_$1_t');

    const hash = crypto.createHash('sha1');

    const vertex = concatenated.length;
    const vertexSource = shaders[key].vertexSource + '\n\0';
    hash.update(vertexSource);
    concatenated += vertexSource;

    const fragment = concatenated.length;
    const fragmentSource = shaders[key].fragmentSource + '\n\0';
    hash.update(fragmentSource);
    concatenated += fragmentSource;

    offsets[key] = {
        vertex,
        fragment,
        originalKey: key,
        hash: hash.digest('hex').substring(0, 16).match(/.{1,2}/g).map(n => `0x${n}`).join(', '),
        shaderName: key.replace(/[A-Z]+/g, (match) => `_${match.toLowerCase()}`),
        ShaderName: key.replace(/^[a-z]/g, (match) => match.toUpperCase())
    };
}

// Overrides: The symbolSDF shader is used for two underlying programs.
offsets.symbolSDFIcon = {
    vertex: offsets.symbolSDF.vertex,
    fragment: offsets.symbolSDF.fragment,
    hash: offsets.symbolSDF.hash,
    originalKey: 'symbolSDF',
    shaderName: 'symbol_sdf_icon',
    ShaderName: 'SymbolSDFIcon',
};

offsets.symbolSDFText = {
    vertex: offsets.symbolSDF.vertex,
    fragment: offsets.symbolSDF.fragment,
    hash: offsets.symbolSDF.hash,
    originalKey: 'symbolSDF',
    shaderName: 'symbol_sdf_text',
    ShaderName: 'SymbolSDFText',
};

delete offsets.symbolSDF;

writeIfModified(path.join(outputPath, 'gl', 'shader_source.hpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#pragma once

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

} // namespace gl
} // namespace programs
} // namespace mbgl
`);

writeIfModified(path.join(outputPath, 'gl', 'preludes.hpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#pragma once

namespace mbgl {
namespace programs {
namespace gl {

extern const char* vertexPrelude;
extern const char* fragmentPrelude;

} // namespace gl
} // namespace programs
} // namespace mbgl
`);

writeIfModified(path.join(outputPath, 'gl', 'preludes.cpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#include <mbgl/programs/gl/preludes.hpp>

namespace mbgl {
namespace programs {
namespace gl {

const char* vertexPrelude = R"MBGL_SHADER(
${shaders.prelude.vertexSource}
)MBGL_SHADER";
const char* fragmentPrelude = R"MBGL_SHADER(
${shaders.prelude.fragmentSource}
)MBGL_SHADER";

} // namespace gl
} // namespace programs
} // namespace mbgl
`);

for (const key in offsets) {
    if (key === 'prelude')
        continue;

    const { shaderName, ShaderName, originalKey } = offsets[key];

    writeIfModified(path.join(outputPath, 'gl', `${shaderName}.cpp`), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/${shaderName}_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<${ShaderName}Program> {
    static constexpr const char* name = "${shaderName}";
    static constexpr const char* vertexSource = R"MBGL_SHADER(
${shaders[originalKey].vertexSource}
)MBGL_SHADER";
    static constexpr const char* fragmentSource = R"MBGL_SHADER(
${shaders[originalKey].fragmentSource}
)MBGL_SHADER";
};

constexpr const char* ShaderSource<${ShaderName}Program>::name;
constexpr const char* ShaderSource<${ShaderName}Program>::vertexSource;
constexpr const char* ShaderSource<${ShaderName}Program>::fragmentSource;

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<${ShaderName}Program>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<${ShaderName}Program>>(programParameters);
}

} // namespace gfx
} // namespace mbgl
`);
}
