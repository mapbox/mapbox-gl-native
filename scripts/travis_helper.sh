#!/usr/bin/env bash

function mapbox_time_start {
    local name=$1
    mapbox_timer_name=$name

    travis_fold start $name

    mapbox_timer_id=$(printf %08x $(( RANDOM * RANDOM )))
    eval "mapbox_start_time_$mapbox_timer_id=$(travis_nanoseconds)"
    echo -en "travis_time:start:$mapbox_timer_id\n"
}

function mapbox_time_finish {
    local name=${1:-$mapbox_timer_name}
    local timer_id=${2:-$mapbox_timer_id}
    local timer_start="mapbox_start_time_$timer_id"
    eval local start_time=\${$timer_start}
    local end_time=$(travis_nanoseconds)
    local duration=$(($end_time-$start_time))
    echo -en "travis_time:end:$timer_id:start=$start_time,finish=$end_time,duration=$duration\n"

    travis_fold end $name
}

function mapbox_time {
    local name=$1 ; shift
    mapbox_time_start $name
    local timer_id=$mapbox_timer_id
    echo "\$ $@"
    $@
    mapbox_time_finish $name $timer_id
}


export ANSI_CLEAR
export -f travis_fold
export -f travis_nanoseconds
export -f mapbox_time
export -f mapbox_time_start
export -f mapbox_time_finish
