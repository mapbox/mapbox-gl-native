#!/usr/bin/env bash
NAME=$1
shift
CMD=$@
DIR="build/binding.xcodeproj/xcshareddata/xcschemes"
mkdir -p "${DIR}"

sed "s#{{NODE_PATH}}#$(dirname `which node`)#;s#{{BLUEPRINT_IDENTIFIER}}#$(hexdump -n 12 -v -e '/1 "%02X"' /dev/urandom)#;s#{{WORKING_DIRECTORY}}#$(pwd)#;s#{{NODE_ARGUMENT}}#${CMD}#" platform/node/scripts/node.xcscheme > "${DIR}/${NAME}.xcscheme"
