#!/usr/bin/env bash

set -e
set -o pipefail
set -u

usage() {
cat <<EOF
# Usage: sh $0 version target_directory [argument]

# version: The semver version plus optional alpha beta distinction (i.e. {major.minor.patch}{-alpha.N})
# target_directory: The directory where build output should be placed

# argument:
#     -g: Upload to github

# environment variables and dependencies: 
#     - You must run "mbx auth ..." before running
#     - Set GITHUB_TOKEN to a GitHub API access token in your environment to use GITHUB_RELEASE
#     - "wget" is required for downloading the zip files from s3
#     - The "github-release" gem is required to use GITHUB_RELEASE
EOF
}

buildPackageStyle() {
    local package=$1 style=""     
    if [[ ${#} -eq 2 ]]; then
        style="$2"
    fi            
    echo "make ${package} ${style}"
    make ${package}
    echo "publish ${package} with ${style}"
    local file_name=""
    if [ -z ${style} ] 
    then
        ./platform/ios/scripts/publish.sh "${PUBLISH_VERSION}"
        file_name=mapbox-ios-sdk-${PUBLISH_VERSION}.zip        
    else
        ./platform/ios/scripts/publish.sh "${PUBLISH_VERSION}" ${style}
        file_name=mapbox-ios-sdk-${PUBLISH_VERSION}-${style}.zip        
    fi
    echo "Downloading ${file_name} from s3... to ${BINARY_DIRECTORY}"
    wget -P ${BINARY_DIRECTORY} http://mapbox.s3.amazonaws.com/mapbox-gl-native/ios/builds/${file_name}
    if [[ "${GITHUB_RELEASE}" == true ]]; then
        echo "publish ${file_name} to GitHub"
        github-release --verbose upload --tag "ios-v${PUBLISH_VERSION}" --name ${file_name} --file "${BINARY_DIRECTORY}/${file_name}"
    fi        
}

if [ ${#} -eq 0 -o ${#} -gt 3 ]; then
    usage
    exit 1
fi

export TRAVIS_REPO_SLUG=mapbox-gl-native
export PUBLISH_VERSION=$1
export GITHUB_USER=mapbox
export GITHUB_REPO=mapbox-gl-native
export BUILDTYPE=Release

BINARY_DIRECTORY=$2
PUBLISH_PRE_FLAG=''
GITHUB_RELEASE=false

echo "Deploying version ${PUBLISH_VERSION}..."

if [[ ${#} -eq 3 &&  $3 == "-g" ]]; then
    GITHUB_RELEASE=true
fi
 
make clean && make distclean

if [[ "${GITHUB_RELEASE}" == true ]]; then
    echo "Create  GitHub release..."
    if [[ $( echo ${PUBLISH_VERSION} | awk '/[0-9]-/' ) ]]; then
        PUBLISH_PRE_FLAG='--pre-release'
    fi
    github-release --verbose release --tag "ios-v${PUBLISH_VERSION}" --name "ios-v${PUBLISH_VERSION}" --draft ${PUBLISH_PRE_FLAG}
fi

buildPackageStyle "ipackage" "symbols"
buildPackageStyle "ipackage-strip"
buildPackageStyle "iframework" "symbols-dynamic"
buildPackageStyle "iframework SYMBOLS=NO" "dynamic"
buildPackageStyle "ifabric" "fabric"
