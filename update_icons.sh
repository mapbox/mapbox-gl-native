#!/bin/bash

export PATH=$HOME/bin:/usr/local/bin:/usr/bin:/bin

if [[ -z `which iconoblast` ]]; then
    echo "Requires https://github.com/mapbox/iconoblast!"
    exit 1
fi

commit=`git rev-parse --short HEAD`
branch=`git rev-parse --abbrev-ref HEAD`
repo=`git remote show origin | grep 'Fetch URL' | sed -e 's/.*github\.com:mapbox\///' -e 's/\.git$//' -e 's/^mapbox-//'`

pwd=`pwd`
cd android/java/app/src/main/res
for folder in `find . -type d -name drawable-\* -maxdepth 1`
do
    cd ${folder}
    cp icon.png icon_burned.png
    iconoblast icon_burned.png $commit $branch $repo
    cd ..
done
cd ${pwd}
