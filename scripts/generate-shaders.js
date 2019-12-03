#!/usr/bin/env node

require('@mapbox/flow-remove-types/register');

const path = require('path');
const outputPath = 'src/mbgl/programs';
const zlib = require('zlib');
const crypto = require('crypto');

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

    const hash = crypto.createHash('sha1');

    const vertex = concatenated.length;
    const vertexSource = basicMinify(shaders[key].vertexSource) + '\n\0';
    hash.update(vertexSource);
    concatenated += vertexSource;

    const fragment = concatenated.length;
    const fragmentSource = basicMinify(shaders[key].fragmentSource) + '\n\0';
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

const compressed = zlib.deflateSync(concatenated, {level: zlib.Z_BEST_COMPRESSION})
    .toString('hex')
    .match(/.{1,32}/g)
    .map(line => line.match(/.{1,2}/g).map(n => `0x${n}`).join(', '))
    .join(',\n    ')
    .trim();

writeIfModified(path.join(outputPath, 'gl', 'shader_source.hpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#pragma once

namespace mbgl {
namespace programs {
namespace gl {

const char* shaderSource();

template <typename>
struct ShaderSource;

} // namespace gl
} // namespace programs
} // namespace mbgl
`);

writeIfModified(path.join(outputPath, 'gl', 'shader_source.cpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/util/compression.hpp>

#include <cstdint>

namespace mbgl {
namespace programs {
namespace gl {

constexpr const uint8_t compressedShaderSource[] = {
    ${compressed}};

const char* shaderSource() {
    static std::string decompressed = util::decompress(std::string(reinterpret_cast<const char*>(compressedShaderSource), sizeof(compressedShaderSource)));
    return decompressed.c_str();
};

} // namespace gl
} // namespace programs
} // namespace mbgl
// clang-format on
`);

writeIfModified(path.join(outputPath, 'gl', 'preludes.hpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#pragma once

#include <cstdint>

namespace mbgl {
namespace programs {
namespace gl {

constexpr const uint8_t preludeHash[8] = {${offsets['prelude'].hash}};
constexpr const auto vertexPreludeOffset = ${offsets['prelude'].vertex};
constexpr const auto fragmentPreludeOffset = ${offsets['prelude'].fragment};

} // namespace gl
} // namespace programs
} // namespace mbgl
`);

for (const key in offsets) {
    if (key === 'prelude')
        continue;

    const { shaderName, ShaderName, originalKey } = offsets[key];

    writeIfModified(path.join(outputPath, 'gl', `${shaderName}.cpp`), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
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
    static constexpr const uint8_t hash[8] = {${offsets[key].hash}};
    static constexpr const auto vertexOffset = ${offsets[key].vertex};
    static constexpr const auto fragmentOffset = ${offsets[key].fragment};
};

constexpr const char* ShaderSource<${ShaderName}Program>::name;
constexpr const uint8_t ShaderSource<${ShaderName}Program>::hash[8];

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

// Uncompressed source of ${shaderName}.vertex.glsl:
/*
${shaders[originalKey].vertexSource}
*/

// Uncompressed source of ${shaderName}.fragment.glsl:
/*
${shaders[originalKey].fragmentSource}
*/
// clang-format on
`);
}
