#!/usr/bin/env bash

export ROOTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

perl -i -p -e "s/\.X/\.x/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/\->X/\->x/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/cInt X;/cInt x;/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/double X;/double x;/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/X\(x\)/x\(_x\)/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/X\(\(/x\(\(/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/double x = 0/double _x = 0/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/cInt x = 0/cInt _x = 0/g;" ${ROOTDIR}/clipper.*

perl -i -p -e "s/\.Y/\.y/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/\->Y/\->y/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/cInt Y;/cInt y;/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/double Y;/double y;/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/Y\(y\)/y\(_y\)/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/Y\(\(/y\(\(/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/cInt          Y;/cInt          y;/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/double y = 0/double _y = 0/g;" ${ROOTDIR}/clipper.*
perl -i -p -e "s/cInt y = 0/cInt _y = 0/g;" ${ROOTDIR}/clipper.*


