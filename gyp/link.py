#!/usr/bin/env python

import sys
from merge_static_libs import MergeLibs

args = sys.argv[1:]

out_lib = ''
in_libs = []
flags = []

i = 0
l = len(args)
while i < l:
    arg = args[i]

    if arg[0:2] == '-l':
        flags.append(arg)
    elif arg == '-arch':
        i += 1
    elif arg == '-framework':
        i += 1
        flags.append(arg + ' ' + args[i])
    elif arg == '-o':
        i += 1
        out_lib = args[i]
    elif arg[0] != '-':
        in_libs.append(arg)
    else:
        print 'Ignored linker directive: ' + arg

    i += 1

MergeLibs(in_libs, out_lib)
