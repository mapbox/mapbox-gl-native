#!/usr/bin/python

###
### Script to configure, schedule and AWS Device Farm runs:
###  -  run `python run-device-farm-ios.py --help` to view required input.
###  -  requires awscli to be installed and configured
###  -  schedules iOS tests on a given Device Farm project
###     and device pool ARN.
###

import os
import argparse
import subprocess
import json
import time
import requests
import zipfile
import StringIO


def filesystemARN(arn):
    for source,dest in {":":"#", "/":"_"}.items():
        arn = arn.replace(source, dest)
    return arn

###
### Step 0: read command line input
###

parser = argparse.ArgumentParser(description='Script to configure, schedule and AWS Device Farm runs.')
parser.add_argument('--project', help= 'Provide AWS Device Farm project ARN')
parser.add_argument('--devicepool', help= 'Provide AWS Device Farm device pool project ARN')
parser.add_argument('--ipa', help= 'Provide path to ipa to test')
parser.add_argument('--test', help= 'Provide path to ipa with tests')
parser.add_argument('--filter', help='Provide filter for test execution. (Optional)')
parser.add_argument('--externaldata', help='Zip archive containing extra data for test run. Unzipped to /sdcard before test run. (Optional)')
parser.add_argument('--testspec', help='Custom test environment specification. (Optional)')
parser.add_argument('--outputdir', help='Directory to download artifacts to')

args = parser.parse_args()

projectArn = args.project
deviceArn = args.devicepool
appPath = args.ipa
testPath = args.test
filter = args.filter
externalDataPath = args.externaldata
testSpecPath = args.testspec
outputdir = args.outputdir

# Wait for an upload/run to complete by polling for a status
def wait_for_request_status(request, status = 'SUCCEEDED', api='upload', polling_interval = 15.0):
    if not request:
        return
    while True:
        # log start time
        start = time.clock()

        try:
            runInfo = subprocess.check_output(['aws', 'devicefarm', 'get-'+api,
            '--arn', request[api]['arn'],
            '--region', 'us-west-2'], stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))

        print runInfo

        runInfo = json.loads(runInfo)

        if (runInfo[api]['status'] == status):
            print api + " completed."
            break
        else :
            print api + " has not completed, retrying in " + str(polling_interval) + " seconds"

            # calculate sleep time
            work_duration = time.clock() - start
            time.sleep( polling_interval - work_duration )


###
### Step 1: upload application under test to AWS Device Farm using create-upload and curl
### https://docs.aws.amazon.com/cli/latest/reference/devicefarm/create-upload.html
###

print "------------------------------------------------------------------------"
print "Step 1: upload application under test to AWS Device Farm"
print "------------------------------------------------------------------------"

head, tail = os.path.split(appPath)
appName = tail

print("appPath="+appPath)
print("appName="+appName)
print("appName="+projectArn)


try:
    uploadAppRequest = subprocess.check_output(['aws', 'devicefarm', 'create-upload',
    '--project-arn', projectArn,
    '--name', appName,
    '--type', 'IOS_APP',
    '--region', 'us-west-2'], stderr=subprocess.STDOUT)
except subprocess.CalledProcessError as e:
    raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))

print uploadAppRequest

uploadAppRequest = json.loads(uploadAppRequest)

print "uploading app.."
uploadApp = subprocess.check_output(['curl', '-T', appPath, uploadAppRequest['upload']['url']], stderr=subprocess.STDOUT)
print uploadApp

wait_for_request_status(uploadAppRequest)


###
### Step 2: upload application containing tests to AWS Device Farm using create-upload and curl
### https://docs.aws.amazon.com/cli/latest/reference/devicefarm/create-upload.html
###

print "------------------------------------------------------------------------"
print "Step 2: upload application containing tests to AWS Device Farm"
print "------------------------------------------------------------------------"

head, tail = os.path.split(testPath)
testName = tail

try:
    uploadTestRequest = subprocess.check_output(['aws', 'devicefarm', 'create-upload',
    '--project-arn', projectArn,
    '--name', testName,
    '--type', 'XCTEST_TEST_PACKAGE',
    '--region', 'us-west-2'], stderr=subprocess.STDOUT)
except subprocess.CalledProcessError as e:
    raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))

print uploadTestRequest

uploadTestRequest = json.loads(uploadTestRequest)

print "uploading tests.."
uploadTest = subprocess.check_output(['curl', '-T', testPath, uploadTestRequest['upload']['url']], stderr=subprocess.STDOUT)
print uploadTest

# https://docs.aws.amazon.com/devicefarm/latest/developerguide/how-to-create-test-run.html#how-to-create-test-run-cli-step5

wait_for_request_status(uploadTestRequest)


###
### Step 3: (optional) upload external data archive EXTERNAL_DATA upload type
### https://docs.aws.amazon.com/cli/latest/reference/devicefarm/create-upload.html
###

print "-------------------------------------------------------------------------"
print "Step 3: (optional) upload external data archive"
print "-------------------------------------------------------------------------"

uploadExternalDataRequest = None
if externalDataPath:
    head, tail = os.path.split(externalDataPath)
    externalDataName = tail

    try:
        uploadExternalDataRequest = subprocess.check_output(['aws', 'devicefarm', 'create-upload',
        '--project-arn', projectArn,
        '--name', externalDataName,
        '--type', 'EXTERNAL_DATA',
        '--region', 'us-west-2'], stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))

    print uploadExternalDataRequest

    uploadExternalDataRequest = json.loads(uploadExternalDataRequest)

    print "uploading external data archive.."
    uploadExternalData = subprocess.check_output(['curl', '-T', externalDataPath, uploadExternalDataRequest['upload']['url']], stderr=subprocess.STDOUT)
    print uploadExternalData
else:
    print "No external data archive was provided, skipping..."


###
### Step 4: (optional) upload custom test environment specification yaml file INSTRUMENTATION_TEST_SPEC upload type
### https://docs.aws.amazon.com/cli/latest/reference/devicefarm/create-upload.html
### https://docs.aws.amazon.com/devicefarm/latest/developerguide/test-environments.html#custom-test-environment
###

print "------------------------------------------------------------------------"
print "Step 4: (optional) upload custom test environment specification"
print "------------------------------------------------------------------------"

uploadTestSpecRequest = None
if testSpecPath:
    head, tail = os.path.split(testSpecPath)
    testSpecName = tail

    try:
        uploadTestSpecRequest = subprocess.check_output(['aws', 'devicefarm', 'create-upload',
        '--project-arn', projectArn,
        '--name', testSpecName,
        '--type', 'XCTEST_UI_TEST_SPEC',
        '--region', 'us-west-2'], stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))

    print uploadTestSpecRequest

    uploadTestSpecRequest = json.loads(uploadTestSpecRequest)

    print "uploading custom test spec.."
    uploadTestSpec = subprocess.check_output(['curl', '-T', testSpecPath, uploadTestSpecRequest['upload']['url']], stderr=subprocess.STDOUT)
    print uploadTestSpec
else:
    print "No custom test specification was provided, skipping..."

wait_for_request_status(uploadTestSpecRequest)


###
### Step 5: schedule run on AWS Device Farm using schedule-run
### https://docs.aws.amazon.com/cli/latest/reference/devicefarm/schedule-run.html
###

print "------------------------------------------------------------------------"
print "Step 5: run tests on AWS Device Farm"
print "------------------------------------------------------------------------"

extraCommands = []
if uploadTestSpecRequest:
    extraCommands.append('testSpecArn='+uploadTestSpecRequest['upload']['arn'])
if filter:
    extraCommands.append('filter="'+filter+'"')
if extraCommands:
    extraCommands = ','+','.join(str(command) for command in extraCommands)
else:
    extraCommands = ''

extraDataPackageArn = None
if uploadExternalDataRequest:
    extraDataPackageArn = 'extraDataPackageArn='+uploadExternalDataRequest['upload']['arn']
else:
    extraDataPackageArn = ''

try:
    uploadTestRequest = subprocess.check_output(['aws', 'devicefarm', 'schedule-run', 
    '--project-arn', projectArn,
    '--app-arn', uploadAppRequest['upload']['arn'],
    '--device-pool-arn', deviceArn,
    '--name', 'ios test',
    '--test', 'type=XCTEST,testPackageArn='+uploadTestRequest['upload']['arn']+extraCommands,
    '--region', 'us-west-2'], stderr=subprocess.STDOUT)
except subprocess.CalledProcessError as e:
    raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))

print uploadTestRequest
uploadTestRequest = json.loads(uploadTestRequest)

print "scheduling tests.."

wait_for_request_status(uploadTestRequest, 'COMPLETED', 'run', 120.0)


###
### Step 7: download artifacts with list-artifacts and curl
### https://docs.aws.amazon.com/cli/latest/reference/devicefarm/list-artifacts.html
###

print "------------------------------------------------------------------------"
print "Step 7: Download artifacts"
print "------------------------------------------------------------------------"

try:
    listArtifacts = subprocess.check_output(['aws', 'devicefarm', 'list-artifacts', 
    '--arn', uploadTestRequest['run']['arn'],
    '--type', 'FILE',
    '--region', 'us-west-2'], stderr=subprocess.STDOUT, universal_newlines=True)
except subprocess.CalledProcessError as e:
    raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))

print listArtifacts

listArtifacts = json.loads(listArtifacts)

if outputdir is None:
    outputdir = "device-farm-output"
    
# download zip and push content to /build
for artifact in listArtifacts['artifacts']:
    if artifact['type'] == "CUSTOMER_ARTIFACT" :
        print "artifact found : " + artifact['url']
        arn = filesystemARN(artifact['arn'])

        zipdir = os.path.join(outputdir, arn)

        print "unzipping contents to " + zipdir
        r = requests.get(artifact['url'], stream=True)
        z = zipfile.ZipFile(StringIO.StringIO(r.content))
        z.extractall(zipdir)

