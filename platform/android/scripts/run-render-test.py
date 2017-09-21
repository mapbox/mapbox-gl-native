#!/usr/bin/python

import os
from shutil import copyfile

path = os.getcwd() + "/platform/android/tests/render/expected/"
dirs = os.listdir(path)
testDir = os.getcwd() + "/platform/android/build/render-test/"
for test in dirs:
    src = os.getcwd() + "/platform/android/tests/render/expected/" + test + "/expected.png"
    expected = testDir + test + "/expected.png"
    actual = testDir + test + "/actual.png"
    output = testDir + test +"/output.png"
    copyfile(src, expected)
    pixelmatch = "node_modules/pixelmatch/bin/pixelmatch " + actual + " " + expected + " " + output + " 0.1"
    print
    print "Pixel match "+ test
    os.system(pixelmatch)
print
