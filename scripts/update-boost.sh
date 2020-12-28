#!/usr/bin/env bash

set -eu

pushd "$BOOST"
if [ ! -f dist/bin/bcp ]; then
    # Build bcp
    ./bootstrap.sh
    ./b2 tools/bcp
fi
popd

# Extract boost modules that we need
rm -rf vendor/boost/include
mkdir -p vendor/boost/include
"$BOOST/dist/bin/bcp" --boost="$BOOST" --scan $(find {src,include,test,platform,bin} -name "*.cpp" -o -name "*.hpp") vendor/boost/include

pushd vendor/boost
VERSION=$(sed -n 's/^#define BOOST_LIB_VERSION "\([^"]*\)"$/\1/p' include/boost/version.hpp)
echo "libboost ${VERSION/_/.} for Mapbox GL Native" > README.md
git add README.md include
popd

echo "If everything works, run:"
echo ""
echo "    git -C vendor/boost checkout master"
echo "    git -C vendor/boost commit -m \"update boost to ${VERSION/_/.}\""
echo "    git -C vendor/boost push"
echo "    git add vendor/boost"
echo ""
echo "and commit."
