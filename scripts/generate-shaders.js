#!/usr/bin/env node

require('flow-remove-types/register');

const path = require('path');
const outputPath = 'src/mbgl/programs/gl';
const zlib = require('zlib');

var shaders = require('../mapbox-gl-js/src/shaders');

require('./style-code');

let concatenated = '';
let offsets = {};

function basicMinify(src) {
    return src = src.trim() // strip whitespace at the start/end
        .replace(/\s*\/\/[^\n]*\n/g, '\n') // strip double-slash comments
        .replace(/\n+/g, '\n') // collapse multi line breaks
        .replace(/\n\s+/g, '\n') // strip identation
        .replace(/\s?([+-\/*=,])\s?/g, '$1') // strip whitespace around operators
        .replace(/([;\(\),\{\}])\n(?=[^#])/g, '$1'); // strip more line breaks
}

for (const key in shaders) {
    // Rename a_*_t uniforms to u_*_t. This is a workaround until we can use
    // https://github.com/mapbox/mapbox-gl-js/pull/8055, which is blocked by
    // https://github.com/mapbox/mapbox-gl-native/issues/13984
    shaders[key].vertexSource = shaders[key].vertexSource.replace(/\ba_(\w+)_t\b/mg, 'u_$1_t');

    const vertex = concatenated.length;
    concatenated += basicMinify(shaders[key].vertexSource);
    concatenated += '\n\0';

    const fragment = concatenated.length;
    concatenated += basicMinify(shaders[key].fragmentSource);
    concatenated += '\n\0';

    offsets[key] = {
        vertex,
        fragment,
        originalKey: key,
        shaderName: key.replace(/[A-Z]+/g, (match) => `_${match.toLowerCase()}`),
        ShaderName: key.replace(/^[a-z]/g, (match) => match.toUpperCase())
    };
}

// Overrides: The symbolSDF shader is used for two underlying programs.
offsets.symbolSDFIcon = {
    vertex: offsets.symbolSDF.vertex,
    fragment: offsets.symbolSDF.fragment,
    originalKey: 'symbolSDF',
    shaderName: 'symbol_sdf_icon',
    ShaderName: 'SymbolSDFIcon',
};

offsets.symbolSDFText = {
    vertex: offsets.symbolSDF.vertex,
    fragment: offsets.symbolSDF.fragment,
    originalKey: 'symbolSDF',
    shaderName: 'symbol_sdf_text',
    ShaderName: 'SymbolSDFText',
};

delete offsets.symbolSDF;

const compressed = zlib.deflateSync(concatenated, {level: zlib.Z_BEST_COMPRESSION})
    .toString('hex')
    .match(/.{1,32}/g)
    .map(line => line.match(/.{1,2}/g).map(n => `0x${n}`).join(', '))
    .join(',\n    ')
    .trim();

function sourceOffset(key, type) {
    return `programs::gl::shaderSource() + ${offsets[key][type]}`
}

writeIfModified(path.join(outputPath, 'shader_source.hpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#pragma once

namespace mbgl {
namespace programs {
namespace gl {

const char* shaderSource();

} // namespace gl
} // namespace programs
} // namespace mbgl
`);

writeIfModified(path.join(outputPath, 'shader_source.cpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/util/compression.hpp>

#include <cstdint>

namespace mbgl {
namespace programs {
namespace gl {

constexpr const uint8_t compressedShaderSource[] = {
    ${compressed}
};

const char* shaderSource() {
    static std::string decompressed = util::decompress(std::string(reinterpret_cast<const char*>(compressedShaderSource), sizeof(compressedShaderSource)));
    return decompressed.c_str();
};

} // namespace gl
} // namespace programs
} // namespace mbgl
`);

writeIfModified(path.join(outputPath, 'preludes.hpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#pragma once

namespace mbgl {
namespace programs {
namespace gl {

extern const char* vertexShaderPrelude;
extern const char* fragmentShaderPrelude;

} // namespace gl
} // namespace programs
} // namespace mbgl
`);

writeIfModified(path.join(outputPath, 'preludes.cpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>

namespace mbgl {
namespace programs {
namespace gl {

const char* vertexShaderPrelude = ${sourceOffset('prelude', 'vertex')};
const char* fragmentShaderPrelude = ${sourceOffset('prelude', 'fragment')};

} // namespace gl
} // namespace programs
} // namespace mbgl
`);

for (const key in offsets) {
    if (key === 'prelude')
        continue;

    const { shaderName, ShaderName, originalKey } = offsets[key];

    writeIfModified(path.join(outputPath, `${shaderName}.cpp`), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/${shaderName}_program.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace gfx {

template <>
std::unique_ptr<Program<${ShaderName}Program>>
Context::createProgram<gl::Context>(const ProgramParameters& programParameters) {
    return gl::Program<${ShaderName}Program>::createProgram(
        reinterpret_cast<gl::Context&>(*this), programParameters, "${shaderName}",
        ${sourceOffset(key, 'vertex')}, ${sourceOffset(key, 'fragment')});
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of ${shaderName}.vertex.glsl:
/*
${shaders[originalKey].vertexSource}
*/

// Uncompressed source of ${shaderName}.fragment.glsl:
/*
${shaders[originalKey].fragmentSource}
*/

`);
}
