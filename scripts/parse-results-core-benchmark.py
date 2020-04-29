###
### Script to parse core benchmark results coming from and AWS Device Farm
###

import json
import git
import time
import re
import requests
import argparse
import datetime
import gzip
import hashlib

###
### Constants
###

tagBuild = "build"
tagMetadata = "metadata"
tagTraces = "traces"
tagCounters = "counters"
tagAttributes = "attributes"
tagName = "name"

###
### Script input
###

parser = argparse.ArgumentParser(description='Script to parse performance tes results from AWS Device Farm.')
parser.add_argument('--build', help= 'Build number of circle-ci job')
parser.add_argument('--input', help= 'Provide path to instrumentation test output')
parser.add_argument('--git', help= 'Provide path to retrieve git information from (eg. upstream dependency submodule)')
args = parser.parse_args()
buildNumber = args.build
outputFolder = args.input
gitInfoPath = args.git

###
### Create output json template
###

created = datetime.datetime.today().strftime('%Y-%m-%d')
result = ""

###
### Load git information from mapbox-gl-native submodule
###

repo = git.Repo(gitInfoPath)
sha = repo.head.object.hexsha
author = repo.head.object.author.name
timestamp = time.strftime("%Y_%m_%d_%H_%M_%S", time.gmtime(repo.head.object.committed_date))
message = repo.head.object.message
splitGitInfoPath = gitInfoPath.split("/")

###
### Add benchmarks data
###

uniqueTestNames = []

with open(outputFolder+"/results.json") as f:
    parseInput = json.load(f)

    for benchmark in parseInput['benchmarks']:
        test = benchmark['name']

        # Validate if test is valid
        if test in uniqueTestNames:
            # tests are executed 3 times for median calculation
            # this results in other tests showing up 3 times
            # only allow 1 unique name entry in output
            continue
        uniqueTestNames.append(test)

        # Create json output template
        output = json.loads('{"created":"'+created+'","name":"android-core","attributes": [ ], "counters": [ ], "build": { }, "metadata" : { }}')

        # Store git information in "build"
        output[tagBuild].update({"build":int(buildNumber)})
        output[tagBuild].update({"project":splitGitInfoPath[len(splitGitInfoPath)-1]})
        output[tagBuild].update({"branch":"master"})
        output[tagBuild].update({"sha":sha})
        output[tagBuild].update({"author":author})
        output[tagBuild].update({"timestamp":timestamp})
        output[tagBuild].update({"message":message})

        # Store test run information in "metadata"
        output[tagMetadata].update({"num_cpus":parseInput["context"]["num_cpus"]})
        output[tagMetadata].update({"mhz_per_cpu":parseInput["context"]["mhz_per_cpu"]})
        output[tagMetadata].update({"cpu_scaling_enabled":parseInput["context"]["cpu_scaling_enabled"]})
        output[tagMetadata].update({"library_build_type":parseInput["context"]["library_build_type"]})

        # Append test attributes in "attributes"
        output[tagAttributes].append({"name":"time_unit","value":benchmark['time_unit']})
        output[tagAttributes].append({"name":"session_id","value":hashlib.sha1(test+buildNumber).hexdigest()})
        output[tagAttributes].append({"name":"test_name","value":test})

        # Append test data in "counters"
        output[tagCounters].append({"name":"real_time","value":benchmark['real_time']})
        output[tagCounters].append({"name":"cpu_time","value":benchmark['cpu_time']})
        output[tagCounters].append({"name":"iterations","value":benchmark['iterations']})
        try:
            output[tagCounters].append({"name":"label","value":benchmark['label']})
        except KeyError:
            pass

        result += json.dumps(output) + "\n"

print (result)

# write output to json file
with open(outputFolder + '/results_'+buildNumber+'.json', 'w') as outfile:
    outfile.write(result)

# write output to zip file, not formatted
with open(outputFolder + '/results_'+buildNumber+'.json', 'rb') as f_in, gzip.open(outputFolder+'/results_'+buildNumber+'.json.gz', 'wb') as f_out:
    f_out.writelines(f_in)

with gzip.open(outputFolder+'/results_'+buildNumber+'.json.gz', mode="wt") as f:
    f.write(result)
