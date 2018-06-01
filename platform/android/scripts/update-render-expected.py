#!/usr/bin/python

import os
from shutil import copyfile

print "Update render test with executed output:"

path = os.getcwd() + "/platform/android/tests/render/expected/"
dirs = os.listdir(path)
testDir = os.getcwd() + "/platform/android/build/render-test/"
for test in dirs:
    dst = os.getcwd() + "/platform/android/tests/render/expected/" + test + "/expected.png"
    actual = testDir + test + "/actual.png"
    print "Copy file from " + actual + " to " + dst 
    copyfile(actual, dst)
