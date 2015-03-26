export PATH=$HOME/bin:/usr/local/bin:/usr/bin

#
# don't burn during Debug builds
#
if [[ ${CONFIGURATION} != "Release" ]]; then
    echo "Skipping icon burning due to ${CONFIGURATION} configuration"
    exit 0
fi

if [ -z "`which iconoblast`" ]; then
    echo "iconoblast not found in path"
    exit 1
fi

echo "Burning app icons..."

#
# retrieve git info
#
commit=`git rev-parse --short HEAD`
branch=`git rev-parse --abbrev-ref HEAD`
repo=`git remote show -n origin | grep 'Fetch URL' | sed -e 's/.*github\.com\/.*\///' -e 's/\.git$//' -e 's/^mapbox-//'`

#
# work directly in app bundle
#
bundle_path="${CONFIGURATION_BUILD_DIR}/${UNLOCALIZED_RESOURCES_FOLDER_PATH}"
echo "working in ${bundle_path}"
cd "${bundle_path}"

for file in `find . -type f -name Icon\*.png`
do
    iconoblast "$file" $commit $branch $repo
done
cd "$OLDPWD"
