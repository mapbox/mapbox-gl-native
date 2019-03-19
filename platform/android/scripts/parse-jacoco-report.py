#!/usr/bin/python

import os
import re
from io import open

reportPath = os.getcwd() + "/platform/android/MapboxGLAndroidSDK/build/reports/jacoco/jacocoTestReport/jacocoTestReport.xml"
with open(reportPath, 'r', encoding='utf-8') as jacocoReport:
    line = jacocoReport.readline().strip()

    # find the last INSTRUCTION coverage report which is a sum of all separate ones
    instructionIndex = line.rfind('type="INSTRUCTION"')
    startIndex = line.find('missed', instructionIndex)
    endIndex = line.find('/>', startIndex)

    # find the missed and covered lines counts
    numbers = re.match(r'missed="(\d+)" covered="(\d+)"', line[startIndex:endIndex])
    missed = int(numbers.group(1))
    covered = int(numbers.group(2))

    # calculate the code coverage percentage
    percentage = round(covered * 100.0 / (missed + covered), 2)
    print("Android tests code coverage: %s%%" % (str(percentage)))

    # invoke the script that send the data to s3
    testEnvironment = "LOCAL"
    if os.environ.get('IS_LOCAL_DEVELOPMENT').lower()=='false':
        testEnvironment = "CI"

    cmd = os.getcwd() + ("/scripts/code-coverage.sh %.2f %s %s" % (percentage, "Android", testEnvironment))
    os.system(cmd)
