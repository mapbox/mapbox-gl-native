# Version of .ycm_extra_conf.py sample file adapted
# for the Mapbox GL Native project.
#
# This file is NOT licensed under the GPLv3, which is the license for the rest
# of YouCompleteMe.
#
# Here's the license text for this file:
#
# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.
#
# In jurisdictions that recognize copyright laws, the author or authors
# of this software dedicate any and all copyright interest in the
# software to the public domain. We make this dedication for the benefit
# of the public at large and to the detriment of our heirs and
# successors. We intend this dedication to be an overt act of
# relinquishment in perpetuity of all present and future rights to this
# software under copyright law.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# For more information, please refer to <http://unlicense.org/>

import os
import subprocess
import sys
import ycm_core


def DirectoryOfThisScript():
    return os.path.dirname(os.path.abspath(__file__))


default_flags = [
    '-std=c++14',
    '-I%s' % (os.path.join(DirectoryOfThisScript(), 'src')),
    '-I%s' % (os.path.join(DirectoryOfThisScript(), 'include')),
]


def MakeRelativePathsInFlagsAbsolute(flags, working_directory):
    if not working_directory:
        return list(flags)

    new_flags = []
    make_next_absolute = False
    path_flags = ['-isystem', '-I', '-iquote', '--sysroot=']

    for flag in flags:
        new_flag = flag

        if make_next_absolute:
            make_next_absolute = False
            if not flag.startswith('/'):
                new_flag = os.path.join(working_directory, flag)

        for path_flag in path_flags:
            if flag == path_flag:
                make_next_absolute = True
                break

            if flag.startswith(path_flag):
                path = flag[len(path_flag):]
                new_flag = path_flag + os.path.join(working_directory, path)
                break

        if new_flag:
            new_flags.append(new_flag)

    return new_flags


def FlagsForFileLinux(filename):
    working_directory = os.path.join('build', 'linux')

    if not os.path.exists(os.path.join(DirectoryOfThisScript(), working_directory)):
        return {
            'flags': default_flags,
            'do_cache': False,
        }

    relative_source_path = filename.replace(DirectoryOfThisScript() + '/', '')
    relative_obj_path = os.path.splitext(relative_source_path)[0] + '.o'

    make_cmd = 'make -pn |grep %s: |grep GYP_CXXFLAGS |cut -d\' \' -f4-' % relative_obj_path

    p = subprocess.Popen([make_cmd], shell=True, stdout=subprocess.PIPE)
    stdout, stderr = p.communicate()

    if p.returncode:
        return {
            'flags': default_flags,
            'do_cache': False,
        }

    flags = stdout.split(' ')
    flags_relative = MakeRelativePathsInFlagsAbsolute(flags, os.path.join(DirectoryOfThisScript(), working_directory))
    flags_final = [flag for flag in flags_relative if flag.startswith("-")]

    return {
        'flags': flags_final,
        'do_cache': True,
    }


def FlagsForFile(filename, **kwargs):
    if sys.platform.startswith('linux'):
        return FlagsForFileLinux(filename)

    return {
        'flags': default_flags,
        'do_cache': False,
    }
