#!/usr/bin/env bash
CMD=$@
DIR="build/binding.xcodeproj/xcshareddata/xcschemes"
FILE="${DIR}/npm ${CMD}.xcscheme"
mkdir -p "${DIR}"

sed "s#{{NODE_PATH}}#$(dirname `which node`)#;s#{{BLUEPRINT_IDENTIFIER}}#$(hexdump -n 12 -v -e '/1 "%02X"' /dev/urandom)#;s#{{WORKING_DIRECTORY}}#$(pwd)#;s#{{NPM_COMMAND}}#${CMD}#" scripts/node/npm.xcscheme > "${FILE}"