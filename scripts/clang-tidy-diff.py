#!/usr/bin/env python
#
#===- clang-tidy-diff.py - ClangTidy Diff Checker ------------*- python -*--===#
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===------------------------------------------------------------------------===#

r"""
ClangTidy Diff Checker
======================

This script reads input from a unified diff, runs clang-tidy on all changed
files and outputs clang-tidy warnings in changed lines only. This is useful to
detect clang-tidy regressions in the lines touched by a specific patch.
Example usage for git/svn users:

  git diff -U0 HEAD^ | clang-tidy-diff.py -p1
  svn diff --diff-cmd=diff -x-U0 | \
      clang-tidy-diff.py -fix -checks=-*,modernize-use-override

"""

import argparse
import json
import re
import subprocess
import sys
import multiprocessing
import os
import threading

is_py2 = sys.version[0] == '2'

if is_py2:
    import Queue as queue
else:
    import queue as queue

def run_tidy(command, lines_by_file, queue, failed_files):
  """Takes filenames out of queue and runs clang-tidy on them."""
  while True:
    name = queue.get()

    line_filter_json = json.dumps([{"name" : name, "lines" : lines_by_file[name]}], separators = (',', ':'))
    if sys.platform == 'win32':
      line_filter_json = re.sub(r'"', r'"""', line_filter_json)
    else:
      line_filter_json = "'" + line_filter_json + "'";

    invocation = list(command)
    invocation.append('-line-filter=' + line_filter_json)
    invocation.append(name)

    sys.stdout.write('Checking differences in {}...\n'.format(name))
    return_code = subprocess.call(' '.join(invocation), shell=True)
    if return_code != 0:
      failed_files.append(name)
    queue.task_done()

def main():
  parser = argparse.ArgumentParser(description=
                                   'Run clang-tidy against changed files, and '
                                   'output diagnostics only for modified '
                                   'lines.')
  parser.add_argument('-clang-tidy-binary', metavar='PATH',
                      default='clang-tidy',
                      help='path to clang-tidy binary')
  parser.add_argument('-p', metavar='NUM', default=0,
                      help='strip the smallest prefix containing P slashes')
  parser.add_argument('-regex', metavar='PATTERN', default=None,
                      help='custom pattern selecting file paths to check '
                      '(case sensitive, overrides -iregex)')
  parser.add_argument('-iregex', metavar='PATTERN', default=
                      r'.*\.(cpp|cc|c\+\+|cxx|c|cl|h|hpp|m|mm|inc)',
                      help='custom pattern selecting file paths to check '
                      '(case insensitive, overridden by -regex)')

  parser.add_argument('-fix', action='store_true', default=False,
                      help='apply suggested fixes')
  parser.add_argument('-checks',
                      help='checks filter, when not specified, use clang-tidy '
                      'default',
                      default='')
  parser.add_argument('-path', dest='build_path',
                      help='Path used to read a compile command database.')
  parser.add_argument('-extra-arg', dest='extra_arg',
                      action='append', default=[],
                      help='Additional argument to append to the compiler '
                      'command line.')
  parser.add_argument('-extra-arg-before', dest='extra_arg_before',
                      action='append', default=[],
                      help='Additional argument to prepend to the compiler '
                      'command line.')
  parser.add_argument('-j', type=int, default=0,
                      help='number of tidy instances to be run in parallel.')
  parser.add_argument('-quiet', action='store_true', default=False,
                      help='Run clang-tidy in quiet mode')
  clang_tidy_args = []
  argv = sys.argv[1:]
  if '--' in argv:
    clang_tidy_args.extend(argv[argv.index('--'):])
    argv = argv[:argv.index('--')]

  args = parser.parse_args(argv)

  # Extract changed lines for each file.
  filename = None
  lines_by_file = {}
  for line in sys.stdin:
    match = re.search('^\+\+\+\ \"?(.*?/){%s}([^ \t\n\"]*)' % args.p, line)
    if match:
      filename = match.group(2)
    if filename == None:
      continue

    if args.regex is not None:
      if not re.match('^%s$' % args.regex, filename):
        continue
    else:
      if not re.match('^%s$' % args.iregex, filename, re.IGNORECASE):
        continue

    match = re.search('^@@.*\+(\d+)(,(\d+))?', line)
    if match:
      start_line = int(match.group(1))
      line_count = 1
      if match.group(3):
        line_count = int(match.group(3))
      if line_count == 0:
        continue
      end_line = start_line + line_count - 1;
      lines_by_file.setdefault(filename, []).append([start_line, end_line])

  if len(lines_by_file) == 0:
    print("No relevant changes found.")
    sys.exit(0)

  # Run clang-tidy on files containing changes.
  command = [args.clang_tidy_binary]
  if args.fix:
    command.append('-fix')
  if args.checks != '':
    command.append('-checks=' + quote + args.checks + quote)
  if args.quiet:
    command.append('-quiet')
  if args.build_path is not None:
    command.append('-p=%s' % args.build_path)
  for arg in args.extra_arg:
      command.append('-extra-arg=%s' % arg)
  for arg in args.extra_arg_before:
      command.append('-extra-arg-before=%s' % arg)
  command.extend(clang_tidy_args)

  try:
    invocation = list(command)
    invocation.append('-')
    subprocess.check_call(invocation)
  except:
    print("Unable to run clang-tidy: {}".format(' '.join(command)))
    sys.exit(1)

  max_task = args.j
  if max_task == 0:
    max_task = multiprocessing.cpu_count()

  return_code = 0
  try:
    # Spin up a bunch of tidy-launching threads.
    task_queue = queue.Queue(max_task)
    # List of files with a non-zero return code.
    failed_files = []
    for _ in range(max_task):
      t = threading.Thread(target=run_tidy,
                           args=(command, lines_by_file, task_queue, failed_files))
      t.daemon = True
      t.start()

    # Fill the queue with files.
    for name in lines_by_file:
      task_queue.put(name)

    # Wait for all threads to be done.
    task_queue.join()
    if len(failed_files):
      return_code = 1

  except KeyboardInterrupt:
    # This is a sad hack. Unfortunately subprocess goes
    # bonkers with ctrl-c and we start forking merrily.
    print('\nCtrl-C detected, goodbye.')
    os.kill(0, 9)
    return_code = 1

  sys.exit(return_code)

if __name__ == '__main__':
  main()
