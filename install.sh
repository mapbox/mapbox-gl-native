#!/bin/bash
cd mapbox-gl-native
git submodule init
git submodule update
# # This takes like 20 mins
time make iframework FORMAT=static BUILDTYPE=Release

