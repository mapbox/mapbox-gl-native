#!/usr/bin/env python

import sys, os, errno, re, subprocess

DEFAULT_TAG = [0, 0, 0]
DEFAULT_REV = 'unknown'

def is_git_repo():
    try:
        subprocess.check_output("git rev-parse", shell=True)
        return True
    except subprocess.CalledProcessError as exc:
        return False

def parse_tag(raw_tag):
    return map(int, re.sub("[^0-9.]", "", raw_tag).split('.'))

def parse_rev(raw_rev):
    return raw_rev[0:8]

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise


output_dir = sys.argv[1]

if is_git_repo():
    raw_tag = subprocess.check_output("git describe --tags --always --abbrev=0", shell=True)
    raw_rev = subprocess.check_output("git rev-parse HEAD", shell=True)

    # When they're identical, the "git describe" can't find a tag and reports the rev instead.
    if raw_tag == raw_rev:
        tag = DEFAULT_TAG
        rev = parse_rev(raw_rev)
    else:
        tag = parse_tag(raw_tag)
        rev = parse_rev(raw_rev)
else:
    tag = DEFAULT_TAG
    rev = DEFAULT_REV


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
