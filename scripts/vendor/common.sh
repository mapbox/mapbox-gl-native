
set -eu

VENDOR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../vendor"; pwd)
CXX=${CXX:-clang++}
CC=${CC:-clang}

function download {
    if [ ! -f "$VENDOR/.cache/$NAME-$VERSION" ]; then
        echo ">> Downloading $1..."
        mkdir -p "$VENDOR/.cache"
        curl --retry 3 -f -S -L $1 -o "$VENDOR/.cache/$NAME-$VERSION.tmp"
        mv "$VENDOR/.cache/$NAME-$VERSION.tmp" "$VENDOR/.cache/$NAME-$VERSION"
    fi
}

function init {
    rm -rf "$VENDOR/$NAME"
    mkdir -p "$VENDOR/$NAME"
    echo $VERSION > "$VENDOR/$NAME/version.txt"
    cd "$VENDOR/$NAME"
}

function extract_gzip {
    echo ">> Unpacking files from $VENDOR/.cache/$NAME-$VERSION..."
    [ ! -z "$(tar --version | grep "GNU tar")" ] && WC="--wildcards" || WC=""
    tar xzf "$VENDOR/.cache/$NAME-$VERSION" $WC --strip-components=${STRIP_COMPONENTS:-1} -C "$VENDOR/$NAME" $@
}

function extract_zip {
    echo ">> Unpacking files from $VENDOR/.cache/$NAME-$VERSION..."
    unzip -qq "$VENDOR/.cache/$NAME-$VERSION" $@ -d "$VENDOR/.cache/staging"
    mv "$VENDOR/.cache/staging"/*/* "$VENDOR/$NAME"
    rm -rf "$VENDOR/.cache/staging"
}

function file_list {
    (cd "$VENDOR/$NAME" && find $@ | LC_ALL=C sort > "$VENDOR/$NAME/files.txt")
}
