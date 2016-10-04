#!/usr/bin/env bash

set -e
set -o pipefail
set -u

# dynamic environment variables:
#     GITHUB_RELEASE=true: Upload to github

# environment variables and dependencies: 
#     - You must run "mbx auth ..." before running
#     - Set GITHUB_TOKEN to a GitHub API access token in your environment to use GITHUB_RELEASE
#     - "wget" is required for downloading the zip files from s3
#     - The "github-release" gem is required to use GITHUB_RELEASE

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
        ./platform/ios/scripts/publish.sh "${PUBLISH_VERSION}"
        file_name=mapbox-ios-sdk-${PUBLISH_VERSION}.zip        
    else
        ./platform/ios/scripts/publish.sh "${PUBLISH_VERSION}" ${style}
        file_name=mapbox-ios-sdk-${PUBLISH_VERSION}-${style}.zip        
    fi
    step "Downloading ${file_name} from s3 to ${BINARY_DIRECTORY}"
    wget -O ${BINARY_DIRECTORY}/${file_name} http://mapbox.s3.amazonaws.com/mapbox-gl-native/ios/builds/${file_name}
    if [[ "${GITHUB_RELEASE}" == true ]]; then
        step "Uploading ${file_name} to GitHub"
        github-release --verbose upload \
            --tag "ios-v${PUBLISH_VERSION}" \
            --name ${file_name} \
            --file "${BINARY_DIRECTORY}/${file_name}" | sed '/^BODY/d; /^GET/d'
    fi        
}

export TRAVIS_REPO_SLUG=mapbox-gl-native
export GITHUB_USER=mapbox
export GITHUB_REPO=mapbox-gl-native
export BUILDTYPE=Release

PUBLISH_VERSION=
BINARY_DIRECTORY='build/ios/deploy'
PUBLISH_PRE_FLAG=''
GITHUB_RELEASE=${GITHUB_RELEASE:-true}

rm -rf ${BINARY_DIRECTORY}
mkdir -p ${BINARY_DIRECTORY}

if [[ ${GITHUB_RELEASE} = true ]]; then
    GITHUB_RELEASE=true # Assign bool, not just a word
fi

if [[ -z ${PUBLISH_VERSION} ]]; then
    step "Determining version number from most recent relevant git tag…"
    PUBLISH_VERSION=$( git describe --tags --match=ios-v*.*.* --abbrev=0 | sed 's/^ios-v//' )
    echo "${PUBLISH_VERSION}"
fi

step "Deploying version ${PUBLISH_VERSION}…"
 
make clean && make distclean

if [[ "${GITHUB_RELEASE}" == true ]]; then
    step "Create GitHub release…"
    if [[ $( echo ${PUBLISH_VERSION} | awk '/[0-9]-/' ) ]]; then
        PUBLISH_PRE_FLAG='--pre-release'
    fi
    github-release --verbose release \
        --tag "ios-v${PUBLISH_VERSION}" \
        --name "ios-v${PUBLISH_VERSION}" \
        --draft ${PUBLISH_PRE_FLAG} \
        | sed '/^BODY/d; /^GET/d'
fi

buildPackageStyle "ipackage" "symbols"
buildPackageStyle "ipackage-strip"
buildPackageStyle "iframework" "symbols-dynamic"
buildPackageStyle "iframework SYMBOLS=NO" "dynamic"
buildPackageStyle "ifabric" "fabric"
