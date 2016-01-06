'''
Uploads an APK and a test APK to AWS Device Farm and runs the Espresso tests.
Exit code is 0 if all tests pass, 1 otherwise. See README.md for details.
'''

from time import sleep
import argparse
import boto3
import requests

'''
Parser
'''

parser = argparse.ArgumentParser(
    prog='Device Farm Runner',
    description='Runs the Espresso tests on AWS Device Farm.',
    formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    add_help=True)

parser.add_argument('--project-arn',
    type=str, help='The project ARN (Amazon Resource Name)')
parser.add_argument('--device-pool-arn',
    type=str, help='The device pool ARN (Amazon Resource Name)')
parser.add_argument('--app-apk-path',
    type=str, help='Path to the app APK')
parser.add_argument('--test-apk-path',
    type=str, help='Path to the tests APK')

args = vars(parser.parse_args())
project_arn = args.get('project_arn')
device_pool_arn = args.get('device_pool_arn')
app_apk_path = args.get('app_apk_path')
test_apk_path = args.get('test_apk_path')

# Check
if not project_arn or not device_pool_arn:
	raise Exception('You need to set both the project and the device pool ARN.')
elif not app_apk_path or not test_apk_path:
	raise Exception('You need to set both the app and test APK path.')

'''
The AWS Device Farm client
'''

client = boto3.client('devicefarm', region_name='us-west-2')

'''
Methods
'''

def upload_apk(apk_name, apk_path, apk_type):
	print 'Starting upload: %s' % apk_type
	result = client.create_upload(
		projectArn=project_arn, name=apk_name, type=apk_type)
	presigned_url = result.get('upload').get('url')
	upload_arn = result.get('upload').get('arn')

	# PUT the file content and wait
	put_file(apk_path=apk_path, presigned_url=presigned_url)
	wait_for_upload(upload_arn=upload_arn)
	return upload_arn

def put_file(apk_path, presigned_url):
	print 'Uploading: %s' % apk_path
	with open(apk_path, 'rb') as f:
		data = f.read()
		result = requests.put(presigned_url, data=data)
		if result.status_code != 200:
			raise Exception(
				'PUT failed with status code: %s' % result.status_code)

def wait_for_upload(upload_arn):
	print 'Checking if the upload succeeded.'
	succeeded = False
	while not succeeded:
		result = client.get_upload(arn=upload_arn)
		status = result.get('upload').get('status')
		succeeded = (status == 'SUCCEEDED')
		if status == 'FAILED':
			raise Exception('Upload failed.')
		elif not succeeded:
			print 'Upload is not ready (status is %s), waiting for 5 seconds.' % status
			sleep(5)

def schedule_run(app_arn, test_arn):
	print 'Scheduling a run.'
	result = client.schedule_run(
		projectArn=project_arn,
	    appArn=app_arn,
	    devicePoolArn=device_pool_arn,
	    name='automated_run',
	    test={ 'type': 'INSTRUMENTATION', 'testPackageArn': test_arn})
	run_arn = result.get('run').get('arn')
	return_code = wait_for_run(run_arn=run_arn)
	return return_code

def wait_for_run(run_arn):
	print 'Checking if the run succeeded.'
	return_code = ''
	succeeded = False
	while not succeeded:
		result = client.get_run(arn=run_arn)
		status = result.get('run').get('status')
		return_code = result.get('run').get('result')
		succeeded = (status == 'COMPLETED')
		if not succeeded:
			print 'Run not completed (status is %s), waiting for 60 seconds.' % status
			sleep(60)
	return return_code

'''
Main flow
'''

# 1. Upload the app APK
app_arn = upload_apk(
	apk_name='app-debug-unaligned.apk',
	apk_path=app_apk_path,
	apk_type='ANDROID_APP')

# 2. Upload the test APK
test_arn = upload_apk(
	apk_name='app-debug-androidTest-unaligned.apk',
	apk_path=test_apk_path,
	apk_type='INSTRUMENTATION_TEST_PACKAGE')

# 3. Schedule the run
return_code = schedule_run(app_arn=app_arn, test_arn=test_arn)
exit_code = 0 if return_code == 'PASSED' else 1
print 'Run completed: %s' % return_code
exit(exit_code)