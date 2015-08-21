#!/usr/bin/env python

import sys
import os
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
    elif arg[0:2] == '-L':
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

flags.reverse()
unique_flags = []
for flag in flags:
    if flag not in unique_flags:
        unique_flags.append(flag)
unique_flags.reverse()

with open(out_lib + '.ldflags', 'w+') as f:
    f.write(' '.join(unique_flags));

MergeLibs(in_libs, out_lib)
