#!/usr/bin/env bash

function mapbox_time {
    local NAME=$1 ; shift
    travis_fold start "${NAME}"
    travis_time_start
    "$@"
    travis_time_finish
    travis_fold end "${NAME}"
}

export -f travis_fold
export -f travis_time_start
export -f travis_time_finish
export -f travis_nanoseconds
export ANSI_CLEAR
export -f mapbox_time
