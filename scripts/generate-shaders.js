#!/usr/bin/env node

require('flow-remove-types/register');

const path = require('path');
const outputPath = 'src/mbgl/shaders';

var shaders = require('../mapbox-gl-js/src/shaders');

require('./style-code');

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
${shaders.prelude.vertexSource}
)MBGL_SHADER";
const char* fragmentPrelude = R"MBGL_SHADER(
${shaders.prelude.fragmentSource}
)MBGL_SHADER";

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

namespace mbgl {
namespace shaders {

const char* ${shaderName}::name = "${shaderName}";
const char* ${shaderName}::vertexSource = R"MBGL_SHADER(
${shaders[key].vertexSource}
)MBGL_SHADER";
const char* ${shaderName}::fragmentSource = R"MBGL_SHADER(
${shaders[key].fragmentSource}
)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
`);
}
