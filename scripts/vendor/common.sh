
set -eu

VENDOR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../vendor"; pwd)
CXX=${CXX:-clang++}
CC=${CC:-clang}

function download {
    if [ ! -f "$VENDOR/.cache/$NAME-$VERSION.tar.gz" ]; then
        echo ">> Downloading $1..."
        mkdir -p "$VENDOR/.cache"
        curl --retry 3 -f -S -L $1 -o "$VENDOR/.cache/$NAME-$VERSION.tar.gz.tmp"
        mv "$VENDOR/.cache/$NAME-$VERSION.tar.gz.tmp" "$VENDOR/.cache/$NAME-$VERSION.tar.gz"
    fi
}

function init {
    rm -rf "$VENDOR/$NAME"
    mkdir -p "$VENDOR/$NAME"
    echo $VERSION > "$VENDOR/$NAME/version.txt"
    cd "$VENDOR/$NAME"
}

function extract {
    echo ">> Unpacking files from $VENDOR/.cache/$NAME-$VERSION.tar.gz..."
    tar xzf "$VENDOR/.cache/$NAME-$VERSION.tar.gz" --strip-components=${STRIP_COMPONENTS:-1} -C "$VENDOR/$NAME" $@
}

function file_list {
    (cd "$VENDOR/$NAME" && find $@ | sort > "$VENDOR/$NAME/files.txt")
}
