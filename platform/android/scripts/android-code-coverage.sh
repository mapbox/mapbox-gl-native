#!/usr/bin/env bash

set -e
set -o pipefail

#
# Get the .txt file generated from the code coverage HTML report
#
cov_result="";
if [ -d platform/android/MapboxGLAndroidSDK/build/reports/jacoco/ ]; then 
    cov_result=platform/android/MapboxGLAndroidSDK/build/reports/jacoco/debug/index.txt
    echo "$cov_result"
    echo "The value of \"cov_result\" is $cov_result."

else
    echo "Coverage file does not exist. Please run tests before executing"
    exit 1
fi

# Retreive the total number of lines in the text file that was created via the coverage report's HTML file
totalNumberOfLinesInDocument=$(wc -l < platform/android/MapboxGLAndroidSDK/build/reports/jacoco/debug/index.txt)
echo "The value of \"totalNumberOfLinesInDocument\" is $totalNumberOfLinesInDocument."

# The sixth line from the last line in the text file, is the number of lines missed. The fifth from the bottom is
# is the total number which exist in the Maps SDK. So to we need to get these numbers, devide the 6th by the 5th,
# and then multiple the product by 100 to get the coverage percentage.

missed_lines_num=$(sed -n "$(($totalNumberOfLinesInDocument-6)),$(($totalNumberOfLinesInDocument-6))p;$(($totalNumberOfLinesInDocument-6))q" platform/android/MapboxGLAndroidSDK/build/reports/jacoco/debug/index.txt)
echo "The value of \"missed_lines_num\" is $missed_lines_num."


total_lines_num=$(sed -n "$(($totalNumberOfLinesInDocument-5)),$(($totalNumberOfLinesInDocument-5))p;$(($totalNumberOfLinesInDocument-5))q" platform/android/MapboxGLAndroidSDK/build/reports/jacoco/debug/index.txt)
echo "The value of \"total_lines_num\" is $total_lines_num."

# Convert the line coverage for the dynamic target to a percentage. Currently, 
# only CI tests are included when calculated code coverage.
# 
# percentage= $"(($missed_lines / $total_lines))" | bc
percentage=$(echo (($missed_lines_num / $total_lines_num)) | bc)

echo "The code coverage percentage is $percentage."


# NEED TO MULTIPLY $percentage BY 100


# Pass info on to generic code-coverage.sh file

# ./././scripts/code-coverage.sh $percentage "Android" "$1"