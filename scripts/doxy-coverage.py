#!/usr/bin/env python
# -*- mode: python; coding: utf-8 -*-

# All files in doxy-coverage are Copyright 2014 Alvaro Lopez Ortega.
#
#   Authors:
#     * Alvaro Lopez Ortega <alvaro@gnu.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from __future__ import print_function

__author__    = "Alvaro Lopez Ortega"
__email__     = "alvaro@alobbs.com"
__copyright__ = "Copyright (C) 2014 Alvaro Lopez Ortega"

import os
import sys
import argparse
import xml.etree.ElementTree as ET

# Defaults
ACCEPTABLE_COVERAGE = 80

# Global
ns = None

def ERROR(*objs):
    print("ERROR: ", *objs, end='\n', file=sys.stderr)

def FATAL(*objs):
	ERROR (*objs)
	sys.exit((1,0)[ns.noerror])

def parse_file(fullpath):
	tree = ET.parse(fullpath)

	sourcefile  = None
	definitions = {}

	for definition in tree.findall("./compounddef//memberdef"):
		# Should it be documented
		if (definition.get('kind') == 'function' and
			definition.get('static') == 'yes'):
			continue

		# Is the definition documented?
		documented = False
		for k in ('briefdescription', 'detaileddescription', 'inbodydescription'):
			if definition.findall("./%s/"%(k)):
				documented = True
				break

		# Name
		d_def = definition.find('./definition')
		d_nam = definition.find('./name')

		if not sourcefile:
			l = definition.find('./location')
			if l is not None:
				sourcefile = l.get('file')

		if d_def is not None:
			name = d_def.text
		elif d_nam is not None:
			name = d_nam.text
		else:
			name = definition.get('id')

		# Aggregate
		definitions[name] = documented

	if not sourcefile:
		sourcefile = fullpath

	return (sourcefile, definitions)


def parse(path):
	index_fp = os.path.join (path, "index.xml")
	if not os.path.exists (index_fp):
		FATAL ("Documentation not present. Exiting.", index_fp)

	tree = ET.parse(index_fp)

	files = {}
	for entry in tree.findall('compound'):
		if entry.get('kind') in ('dir'):
			continue

		file_fp = os.path.join (path, "%s.xml" %(entry.get('refid')))
		tmp = parse_file (file_fp)
		files[tmp[0]] = tmp[1]

	return files


def report (files, exclude_dirs):
	def get_coverage (f):
		defs = files[f]
		if not defs:
			return 100

		doc_yes = len([d for d in defs.values() if d])
		doc_no  = len([d for d in defs.values() if not d])
		return (doc_yes * 100.0 / (doc_yes + doc_no))

	def file_cmp (a,b):
		return cmp(get_coverage(a), get_coverage(b))

	files_sorted = files.keys()
	files_sorted.sort(file_cmp)
	files_sorted.reverse()

	total_yes = 0
	total_no  = 0

	for f in files_sorted:
		skip = False
		for exclude_dir in exclude_dirs:
			if exclude_dir in f:
				skip = True
				break
		if skip:
			continue

		defs = files[f]
		if not defs:
			continue

		doc_yes = len([d for d in defs.values() if d])
		doc_no  = len([d for d in defs.values() if not d])
		doc_per = doc_yes * 100.0 / (doc_yes + doc_no)

		total_yes += doc_yes
		total_no  += doc_no

		print ('%3d%% - %s - (%d of %d)'%(doc_per, f, doc_yes, (doc_yes + doc_no)))

		defs_sorted = defs.keys()
		defs_sorted.sort()
		for d in defs_sorted:
			if not defs[d]:
				print ("\t", d)

	total_all = total_yes + total_no
	total_per = total_yes * 100 / total_all
	print()
	print("%d%% API documentation coverage" %(total_per))
	return (ns.threshold - total_per, 0)[total_per > ns.threshold]


def main():
	# Arguments
	parser = argparse.ArgumentParser()
	parser.add_argument ("dir",         action="store",      help="Path to Doxygen's XML doc directory")
	parser.add_argument ("--noerror",   action="store_true", help="Do not return error code after execution")
	parser.add_argument ("--threshold", action="store",      help="Min acceptable coverage percentage (Default: %s)"%(ACCEPTABLE_COVERAGE), default=ACCEPTABLE_COVERAGE, type=int)
	parser.add_argument("--excludedirs", nargs='+', help="List of directories to be excluded from coverage analysis", type=str, default=[])


	global ns
	ns = parser.parse_args()
	if not ns:
		FATAL ("ERROR: Couldn't parse parameters")

	# Parse
	files = parse (ns.dir)

	# Print report
	err = report (files, ns.excludedirs)
	if ns.noerror:
		return

	sys.exit(err)


if __name__ == "__main__":
	main()
