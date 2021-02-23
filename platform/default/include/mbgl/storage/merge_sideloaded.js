var fs = require('fs');
fs.writeFileSync('platform/default/mbgl/storage/merge_sideloaded.hpp', `#pragma once

// THIS IS A GENERATED FILE; EDIT merge_sideloaded.sql INSTEAD
// To regenerate, run \`node platform/default/mbgl/storage/merge_sideloaded.js\`

namespace mbgl {

static constexpr const char* mergeSideloadedDatabaseSQL =
${fs.readFileSync('platform/default/mbgl/storage/merge_sideloaded.sql', 'utf8')
    .replace(/ *--.*/g, '')
    .split('\n')
    .filter(a => a)
    .map(line => '"' + line + '\\n"')
    .join('\n')
}
;

} // namespace mbgl
`);

