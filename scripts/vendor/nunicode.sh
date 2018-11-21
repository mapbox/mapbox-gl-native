#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

NAME=nunicode
VERSION=1.8
ROOT=alekseyt-nunicode-246bb27014ab

download "https://bitbucket.org/alekseyt/nunicode/get/$VERSION.tar.gz"
init
extract_gzip "$ROOT/libnu/*.c" "$ROOT/libnu/*.h" "$ROOT/LICENSE"

# Augment config.h with just the defines we need.
head -n 2 libnu/config.h > config.h
cat <<CONFIG >> config.h

// Hardcoded defines for vendored copy
#define NU_WITH_UTF8
#define NU_WITH_TOUPPER
#define NU_WITH_TOLOWER
#define NU_WITH_UNACCENT
#define NU_WITH_Z_COLLATION
CONFIG
tail -n +3 libnu/config.h >> config.h
mv config.h libnu/config.h

# List all files we want to extract
FILES=(
    libnu/strcoll.h
    libnu/strcoll.c

    libnu/ducet.h
    libnu/ducet.c

    libnu/unaccent.h
    libnu/tounaccent.c

    libnu/casemap.h
    libnu/toupper.c
    libnu/tolower.c

    libnu/strings.h
    libnu/strings.c

    libnu/utf8.h
    libnu/utf8.c
)

# Find dependencies for all of these files
echo ">> Finding dependencies..."
ALL=()
for FILE in "${FILES[@]}"; do
    ALL+=($($CC -std=c11 -Ilibnu -c "$FILE" -M | sed -e 's/^[a-z0-9._-]*: *//;s/ *\\$//'))
done

# Remove duplicates
IFS=$'\n' ALL=($(sort <<< "${ALL[*]}" | uniq | sed -n '/^libnu\//p'))
unset IFS

echo ">> Copying files..."

mkdir -p include/libnu src/libnu/gen
for FILE in "${ALL[@]}"; do
    [[ "$FILE" = *.h ]] && DIR="include" || DIR="src"
    # Copy file and replace #include "*.h" with #include <libnu/*.h> so that we can separate
    # includes and source-only files.
    sed 's/^#include \"\([^\"]*.h\)\"/#include <libnu\/\1>/' "$FILE" > "$DIR/$FILE"
done

rm -rf libnu
file_list include src -name "*.h" -o -name "*.c" -not -path "*/gen/*"
