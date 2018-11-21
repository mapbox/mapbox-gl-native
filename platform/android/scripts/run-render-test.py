#!/usr/bin/python

import os
import shutil

catPath = os.getcwd() + "/platform/android/build/render-test/"
failCounter = 0
testCounter = 0
for cat in os.listdir(catPath):
    testPath = catPath + cat + "/"
    for test in os.listdir(testPath):
        inputPath = os.getcwd() + "/mapbox-gl-js/test/integration/render-tests/" + cat + "/" + test
        outputPath = testPath + test

        expected = outputPath + "/expected.png"
        actual = outputPath + "/actual.png"
        output = outputPath + "/output.png"

        shutil.copyfile(inputPath + "/expected.png", expected)
        shutil.copyfile(inputPath + "/style.json", outputPath + "/style.json")
        pixelmatchCommand = "node_modules/pixelmatch/bin/pixelmatch " + actual + " " + expected + " " + output + " 0.1"

        print "Pixel match " + cat + " " + test

        with os.popen(pixelmatchCommand) as f:
            for line_terminated in f:
                line = line_terminated.rstrip('\n')
                pair = line.split(": ")
                if pair[0] == "error":
                    testCounter += 1
                    currentError = pair[1]
                    if currentError != "0%":
                       failCounter += 1
                    # below deletes successful results
                    #    print cat + " " + test + " " + currentError
                    # else:
                        # try:
                        #     if os.path.isdir(outputPath):
                        #         shutil.rmtree(outputPath)
                        # except OSError, e:
                        #     print ("Error: %s - %s." % (e.filename, e.strerror))
print
print  str(failCounter) + "/" + str(testCounter)
