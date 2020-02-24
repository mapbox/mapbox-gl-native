#!/bin/sh

# Xcode generator doesn't include the compiler as the
# first argument, Ninja and Makefiles do. Handle both cases.
if [[ "$1" = "${CMAKE_C_COMPILER}" ]] ; then
    shift
fi

exec "${C_LAUNCHER}" "${CMAKE_C_COMPILER}" "$@"
