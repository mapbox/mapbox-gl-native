#!/usr/bin/env python

import fcntl, os, subprocess, sys, errno

# from http://stackoverflow.com/a/600612
def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

def flock(lockfile, cmd_list, verbose = False):
    mkdir_p(os.path.dirname(lockfile))
    fd = os.open(lockfile, os.O_RDONLY | os.O_NOCTTY | os.O_CREAT, 0o666)
    fcntl.flock(fd, fcntl.LOCK_EX)
    if verbose:
        print(' '.join(cmd_list))
    return subprocess.call(cmd_list)

if '__main__' == __name__:
    try:
        if sys.argv[1] == '-v':
            sys.exit(flock(sys.argv[2], sys.argv[3:], True))
        else:
            sys.exit(flock(sys.argv[1], sys.argv[2:]))
    except KeyboardInterrupt:
        sys.exit(1)
