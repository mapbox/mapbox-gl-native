var fs = require('fs');
fs.writeFileSync('platform/default/mbgl/storage/offline_schema.hpp', `#pragma once

// THIS IS A GENERATED FILE; EDIT offline_schema.sql INSTEAD
// To regenerate, run \`node platform/default/mbgl/storage/offline_schema.js\`

namespace mbgl {

static constexpr const char* offlineDatabaseSchema =
${fs.readFileSync('platform/default/mbgl/storage/offline_schema.sql', 'utf8')
    .replace(/ *--.*/g, '')
    .split('\n')
    .filter(a => a)
    .map(line => '"' + line + '\\n"')
    .join('\n')
}
;

} // namespace mbgl
`);

