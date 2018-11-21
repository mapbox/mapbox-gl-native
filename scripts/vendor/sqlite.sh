#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

NAME=sqlite
VERSION=3.24.0
ROOT=sqlite-amalgamation-3240000

download "https://www.sqlite.org/2018/$ROOT.zip"
init
extract_zip "$ROOT/sqlite3.*"
mkdir -p include src
mv sqlite3.h include
mv sqlite3.c src
file_list include src -name "*.h" -o -name "*.c"
