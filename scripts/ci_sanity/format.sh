docker run --rm -v $PWD:/src:ro tmpsantos/mbgl_ci:1.5 /bin/bash -c 'git diff -U0 --no-color origin/master... *.cpp *.hpp /src |clang-format-diff-8 -p1'
