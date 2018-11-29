#!/usr/bin/env bash

set -e
set -o pipefail
set -u

# dynamic environment variables:
#     VERSION_TAG={determined automatically}: Version tag in format ios-vX.X.X-pre.X
#     GITHUB_RELEASE=true: Upload to github
#     BINARY_DIRECTORY=build/ios/deploy: Directory in which to save test packages

# environment variables and dependencies:
#     - You must run "mbx auth ..." before running
#     - Set GITHUB_TOKEN to a GitHub API access token in your environment to use GITHUB_RELEASE
#     - "wget" is required for downloading the zip files from s3
#     - The "github-release" command is required to use GITHUB_RELEASE

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

buildPackageStyle() {
    local package=$1 style=""
    if [[ ${#} -eq 2 ]]; then
        style="$2"
    fi
    step "Building: make ${package} ${style}"
    make ${package}
    step "Publishing ${package} with ${style}"
    local file_name=""
    if [ -z ${style} ]
    then
        ./platform/ios/scripts/publish-cn.sh "${PUBLISH_VERSION}"
        file_name=mapbox-ios-sdk-${PUBLISH_VERSION}.zip
    else
        ./platform/ios/scripts/publish-cn.sh "${PUBLISH_VERSION}" ${style}
        file_name=mapbox-ios-sdk-${PUBLISH_VERSION}-${style}.zip
    fi
PUBLISH_VERSION=$( echo ${VERSION_TAG} | sed 's/^ios-v//' )
git checkout ${VERSION_TAG}

step "Deploying version ${PUBLISH_VERSION}…"
# 
make clean && make distclean
npm install --ignore-scripts
mkdir -p ${BINARY_DIRECTORY}

buildPackageStyle "iframework" "symbols-dynamic"
buildPackageStyle "iframework SYMBOLS=NO" "dynamic"

step "Finished deploying ${PUBLISH_VERSION} in $(($SECONDS / 60)) minutes and $(($SECONDS % 60)) seconds"
