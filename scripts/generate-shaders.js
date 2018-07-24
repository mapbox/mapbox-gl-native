#!/usr/bin/env node

require('flow-remove-types/register');

const path = require('path');
const outputPath = 'src/mbgl/shaders';
const zlib = require('zlib');

var shaders = require('../mapbox-gl-js/src/shaders');

delete shaders.lineGradient;

require('./style-code');

function compressedData(src) {
    return zlib.deflateSync(src, {level: zlib.Z_BEST_COMPRESSION})
        .toString('hex')
        .match(/.{1,16}/g)
        .map(line => line.match(/.{1,2}/g).map(n => `0x${n}`).join(', '))
        .join(',\n        ')
        .trim();
}

function compressedConstant(data) {
    return `[] () {
    static const uint8_t compressed[] = {
        ${compressedData(data)}
    };
    static std::string decompressed = util::decompress(std::string(reinterpret_cast<const char*>(compressed), sizeof(compressed)));
    return decompressed.c_str();
}()`
}

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
#include <mbgl/util/compression.hpp>

#include <cstdint>

namespace mbgl {
namespace shaders {

const char* vertexPrelude = ${compressedConstant(shaders.prelude.vertexSource)};
const char* fragmentPrelude = ${compressedConstant(shaders.prelude.fragmentSource)};

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
#include <mbgl/util/compression.hpp>

#include <cstdint>

namespace mbgl {
namespace shaders {

const char* ${shaderName}::name = "${shaderName}";
const char* ${shaderName}::vertexSource = ${compressedConstant(shaders[key].vertexSource)};
const char* ${shaderName}::fragmentSource = ${compressedConstant(shaders[key].fragmentSource)};

} // namespace shaders
} // namespace mbgl
`);
}
