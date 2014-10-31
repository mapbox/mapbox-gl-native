#!/usr/bin/env bash

shopt -s expand_aliases

git submodule update --init .mason

alias mason=`pwd`/.mason/mason
export MASON_DIR=`pwd`/.mason
