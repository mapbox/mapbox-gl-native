#!/usr/bin/python

from subprocess import call
from subprocess import Popen, PIPE
import sys

## Run license generation
call('cd ../../ && make android-license', shell=True)

## Git diff changes
p = Popen(['git', 'diff', '--name-only', 'LICENSE.md'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
output, err = p.communicate(b"input data that is passed to subprocess' stdin")
if b"platform/android/LICENSE.md" in output:
   raise ValueError("""An error ocurred while validating the license generation. 
            Changes were detected to the license generation output 
            but weren't commited. Run make android-license and 
            commit the changeset to make this validation pass.""")