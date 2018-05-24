#!/usr/bin/python

import os
from shutil import copyfile

path = os.getcwd() + "/platform/android/tests/render/expected/"
dirs = os.listdir(path)

catPath = os.getcwd() + "/platform/android/build/render-test/mapbox/"
catDir = os.listdir(catPath)

for cat in catDir:
    testPath = catPath + cat + "/"
    testDir = os.listdir(testPath)
    for test in testDir:
        src = os.getcwd() + "/mapbox-gl-js/test/integration/render-tests/"+cat+"/"+ test + "/expected.png"
        json = os.getcwd() + "/mapbox-gl-js/test/integration/render-tests/"+cat+"/"+ test + "/style.json"
        expected = testPath + test + "/expected.png"
        actual = testPath + test + "/actual.png"
        output = testPath + test +"/output.png"
        copyfile(src, expected)
        copyfile(json, testPath + test + "/style.json")
        pixelmatch = "node_modules/pixelmatch/bin/pixelmatch " + actual + " " + expected + " " + output + " 0.1"
        print
        print "Pixel match "+ cat + " " + test
        os.system(pixelmatch)
print
