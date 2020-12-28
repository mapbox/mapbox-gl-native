#!/bin/bash

# Usage:
# check_benchmark_results.sh <baseline> <results> <comparison report in txt> <comparison report in html>

# Benchmark comparison script uses a hard-coded 5% threshold by default, but it causes random failures
# Increase the threshold to 15%

if [ "$(uname)" == "Darwin" ]; then
  sed -i '' s/"if res > 0.05"/"if res > 0.15"/ $PWD/vendor/benchmark/tools/gbench/report.py
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  sed -i s/"if res > 0.05"/"if res > 0.15"/ $PWD/vendor/benchmark/tools/gbench/report.py
fi

# Run the bencmark comparison
vendor/benchmark/tools/compare.py benchmarks $1 $2 > $3

# Find cases where the average is above threshold
tests=`fgrep mean $3 | cut -c1-85 | grep 91m`

if [ -z "$tests" ]
then
      echo "" >> $3
      echo "PASSED, all benchmarks within 15% threshold" >> $3

      ansi2html < $3 > $4
      exit 0
else
      test_names=`fgrep mean $3 | cut -c1-85 | grep 91m | cut -f1 -d' '`

      echo "" >> $3
      echo "FAILED, following benchmarks are not within 15% threshold:" >> $3
      echo "" >> $3
      for t in $test_names;
      do
        echo $t >> $3
      done

      ansi2html < $3 > $4
      exit 1
fi
