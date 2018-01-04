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
        github-release upload \
            --tag "ios-v${PUBLISH_VERSION}" \
            --name ${file_name} \
            --file "${BINARY_DIRECTORY}/${file_name}" > /dev/null
    fi
}

export TRAVIS_REPO_SLUG=mapbox-gl-native
export GITHUB_USER=mapbox
export GITHUB_REPO=mapbox-gl-native
export BUILDTYPE=Release

VERSION_TAG=${VERSION_TAG:-''}
PUBLISH_VERSION=
BINARY_DIRECTORY=${BINARY_DIRECTORY:-build/ios/deploy}
GITHUB_RELEASE=${GITHUB_RELEASE:-true}
PUBLISH_PRE_FLAG=''

if [[ -z `which github-release` ]]; then
    step "Installing github-release…"
    brew install github-release
    if [ -z `which github-release` ]; then
        echo "Unable to install github-release. See: https://github.com/aktau/github-release"
        exit 1
    fi
fi

if [[ ${GITHUB_RELEASE} = "true" ]]; then
    GITHUB_RELEASE=true # Assign bool, not just a string
fi

if [[ -z ${VERSION_TAG} ]]; then
    step "Determining version number from most recent relevant git tag…"
    VERSION_TAG=$( git describe --tags --match=ios-v*.*.* --abbrev=0 )
    echo "Found tag: ${VERSION_TAG}"
fi

if [[ $( echo ${VERSION_TAG} | grep --invert-match ios-v ) ]]; then
    echo "Error: ${VERSION_TAG} is not a valid iOS version tag"
    echo "VERSION_TAG should be in format: ios-vX.X.X-pre.X"
    exit 1
fi

if github-release info --tag ${VERSION_TAG} | grep --quiet "draft: ✗"; then
    echo "Error: ${VERSION_TAG} has already been published on GitHub"
    echo "See: https://github.com/${GITHUB_USER}/${GITHUB_REPO}/releases/tag/${VERSION_TAG}"
    exit 1
fi

PUBLISH_VERSION=$( echo ${VERSION_TAG} | sed 's/^ios-v//' )
git checkout ${VERSION_TAG}

step "Deploying version ${PUBLISH_VERSION}…"

make clean && make distclean
mkdir -p ${BINARY_DIRECTORY}

if [[ "${GITHUB_RELEASE}" == true ]]; then
    step "Create GitHub release…"
    if [[ $( echo ${PUBLISH_VERSION} | awk '/[0-9]-/' ) ]]; then
        PUBLISH_PRE_FLAG='--pre-release'
    fi
    github-release release \
        --tag "ios-v${PUBLISH_VERSION}" \
        --name "ios-v${PUBLISH_VERSION}" \
        --draft ${PUBLISH_PRE_FLAG}
fi

buildPackageStyle "ipackage" "symbols"
buildPackageStyle "ipackage-strip"
buildPackageStyle "iframework" "symbols-dynamic"
buildPackageStyle "iframework SYMBOLS=NO" "dynamic"

step "Finished deploying ${PUBLISH_VERSION} in $(($SECONDS / 60)) minutes and $(($SECONDS % 60)) seconds"
