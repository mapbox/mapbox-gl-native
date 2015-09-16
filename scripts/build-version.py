#!/usr/bin/env python

import sys, os, errno, re


output_dir = sys.argv[1]

if len(sys.argv) <= 3:
    tag = [0, 0, 0]
    rev = sys.argv[2][0:8]
else:
    # When they're identical, the git describe can't find a tag and reports the rev instead.
    if sys.argv[2] == sys.argv[3]:
        tag = [0, 0, 0]
    else:
        ver = re.sub("[^0-9.]", "", sys.argv[2])
        tag = map(int, ver.split('.'))
        rev = sys.argv[3][0:8]


def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise


header = """// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#ifndef MBGL_UTIL_VERSION
#define MBGL_UTIL_VERSION

#define MBGL_VERSION 0x{major:02x}{minor:02x}{patch:02x}
#define MBGL_VERSION_STRING "{major}.{minor}.{patch}"
#define MBGL_VERSION_MAJOR {major}
#define MBGL_VERSION_MINOR {minor}
#define MBGL_VERSION_PATCH {patch}
#define MBGL_VERSION_REV "{rev}"

namespace mbgl {{
namespace version {{

extern const int major, minor, patch;
extern const char *revision;
extern const char *string;
extern const unsigned int number;

}}
}}

#endif
""".format(
    major = tag[0],
    minor = tag[1],
    patch = tag[2],
    rev = rev
)

header_path = os.path.join(output_dir, 'include/mbgl/util/version.hpp')
mkdir_p(os.path.dirname(header_path))
with open(header_path, 'w') as f: f.write(header)

