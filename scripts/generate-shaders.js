#!/usr/bin/env node

require('flow-remove-types/register');

const path = require('path');
const outputPath = 'src/mbgl/shaders';
const zlib = require('zlib');

var shaders = require('../mapbox-gl-js/src/shaders');

require('./style-code');

let concatenated = '';
let offsets = {};

for (const key in shaders) {
    const vertex = concatenated.length;
    concatenated += shaders[key].vertexSource;
    concatenated += '\0';

    const fragment = concatenated.length;
    concatenated += shaders[key].fragmentSource;
    concatenated += '\0';

    offsets[key] = {vertex, fragment};
}

const compressed = zlib.deflateSync(concatenated, {level: zlib.Z_BEST_COMPRESSION})
    .toString('hex')
    .match(/.{1,16}/g)
    .map(line => line.match(/.{1,2}/g).map(n => `0x${n}`).join(', '))
    .join(',\n        ')
    .trim();

function sourceOffset(key, type) {
    return `source() + ${offsets[key][type]}`
}

writeIfModified(path.join(outputPath, 'source.hpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#pragma once

namespace mbgl {
namespace shaders {

const char* source();

} // namespace shaders
} // namespace mbgl
`);

writeIfModified(path.join(outputPath, 'source.cpp'), `// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/source.hpp>
#include <mbgl/util/compression.hpp>

#include <cstdint>

namespace mbgl {
namespace shaders {

const char* source() {
    static const uint8_t compressed[] = {
        ${compressed}
    };
    static std::string decompressed = util::decompress(std::string(reinterpret_cast<const char*>(compressed), sizeof(compressed)));
    return decompressed.c_str();
};

} // namespace shaders
} // namespace mbgl
`);

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
#include <mbgl/shaders/source.hpp>

namespace mbgl {
namespace shaders {

const char* vertexPrelude = ${sourceOffset('prelude', 'vertex')};
const char* fragmentPrelude = ${sourceOffset('prelude', 'fragment')};

} // namespace shaders
} // namespace mbgl
`);

for (const key in shaders) {
    if (key === 'prelude')
        continue;

    const shaderName = key.replace(/[A-Z]+/g, (match) => `_${match.toLowerCase()}`);

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
#include <mbgl/shaders/source.hpp>

namespace mbgl {
namespace shaders {

const char* ${shaderName}::name = "${shaderName}";
const char* ${shaderName}::vertexSource = ${sourceOffset(key, 'vertex')};
const char* ${shaderName}::fragmentSource = ${sourceOffset(key, 'fragment')};

// Uncompressed source of ${shaderName}.vertex.glsl:
/*
${shaders[key].vertexSource}
*/

// Uncompressed source of ${shaderName}.fragment.glsl:
/*
${shaders[key].fragmentSource}
*/

} // namespace shaders
} // namespace mbgl
`);
}
