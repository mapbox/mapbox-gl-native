#!/usr/bin/env bash

set -e
set -o pipefail

# Treats all characters as bytes, removes Unicode awareness from (broken?) macOS sed.
export LC_ALL=C

if [ `uname -s` = 'Darwin' ]; then
    # BSD sed accepts a file extension.
    SED="sed -i''"
else
    # GNU sed does not.
    SED="sed -i"
fi

# Gather list of files
FILES=$(git ls-files "*.hpp" "*.cpp" "*.h" "*.mm" "*.m" "*.c" "*.java" "*.xml" "*.ejs" "*.gradle" |
        sed '/^platform\/android\/MapboxGLAndroidSDK\/src\/main\/java\/com\/almeros\// d;/^src\/clipper\// d;/^platform\/ios\/uitest\/OCMock/d')

# Adds trailing newlines to files.
echo "Checking for missing trailing newlines..."
echo "${FILES}" | tr '\n' '\0' | xargs -0 ${SED} -e '$a\'

# Removes trailing whitespace.
echo "Checking for trailing whitespace..."
echo "${FILES}" | tr '\n' '\0' | xargs -0 ${SED} 's/ *$// '

# Add space after // comments. Does not replace // within double quotes.
# Part of the regex is from http://stackoverflow.com/a/11503678
echo "Checking for missing spaces after comments..."
echo "${FILES}" | tr '\n' '\0' | xargs -0 perl -p -i -e 's/(?<!(:|\/))\/\/(?=[^ \/\n])(?=(?:[^"]*"[^"]*")*[^"]*\Z)/\/\/ /g'

git diff --exit-code -- ${FILES} || {
    echo "Some files were modified during code style checking."
    exit 1
}
