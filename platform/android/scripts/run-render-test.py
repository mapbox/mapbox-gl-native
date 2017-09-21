#!/usr/bin/python

import os
from shutil import copyfile

catPath = os.getcwd() + "/platform/android/build/render-test/render/"
for cat in os.listdir(catPath):
    testPath = catPath + cat + "/"
    for test in os.listdir(testPath):
        inputPath = os.getcwd() + "/mapbox-gl-js/test/integration/render-tests/" + cat + "/" + test
        outputPath = testPath + test

        expected = outputPath + "/expected.png"
        actual = outputPath + "/actual.png"
        output = outputPath + "/output.png"

        copyfile(inputPath + "/expected.png", expected)
        copyfile(inputPath + "/style.json", outputPath + "/style.json")
        pixelmatch = "node_modules/pixelmatch/bin/pixelmatch " + actual + " " + expected + " " + output + " 0.1"

        print
        print "Pixel match "+ cat + " " + test
        os.system(pixelmatch)
print
