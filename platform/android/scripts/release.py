'''
Utility to schedule SDK builds on CircleCI.

Examples:

- Publish a snapshot from master (release.py uses the current branch)

	$ git branch
	* master
	  1234-fix-crash
	$ python release.py --stage snapshot

- Publish a snapshot from a feature branch (same as before, just switch branchs with git):

	$ git branch
	  master
	* 1234-fix-crash
	$ python release.py --stage snapshot

- Publish a beta from a pre-release branch:

	$ git branch
	  master
	* release-android-420-beta1
	$ python release.py --stage beta --version 4.2.0-beta.1

- Publish a beta from a release branch:

	$ git branch
	  master
	* release-android-420
	$ python release.py --stage final --version 4.2.0

TODO:

- Add a flag to wait until the release has been built (CircleCI) and published (Maven).

'''

import click
import json
import os
import requests
import subprocess

# Three stages, from less stable to more stable
ALLOWED_STAGES = ['snapshot', 'beta', 'final']

# Get the version from GRADLE_PROPERTIES_PATH below
CURRENT_VERSION_TAG = 'current'

# You can add your API token on https://circleci.com/account/api
CIRCLECI_API_TOKEN_ENV_VAR = 'CIRCLECI_API_TOKEN'

# In the future we might want to consider alpha, or rc.
ALLOWED_PRE_RELEASE = ['beta']

# We get the default version from here
MAPBOX_GL_ANDROID_SDK_PATH = '../MapboxGLAndroidSDK'
GRADLE_PROPERTIES_PATH = '%s/gradle.properties' % MAPBOX_GL_ANDROID_SDK_PATH
GRADLE_TOKEN = 'VERSION_NAME='
FABRIC_PROPERTIES_PATH = '%s/src/main/resources/fabric/com.mapbox.mapboxsdk.mapbox-android-sdk.properties' % MAPBOX_GL_ANDROID_SDK_PATH
FABRIC_TOKEN = 'fabric-version='

# Triggers a new build, returns a summary of the build
URL_CIRCLECI = 'https://circleci.com/api/v1.1/project/github/mapbox/mapbox-gl-native/tree/'  # + :branch

# We support three parameters: stage, branch, and version
@click.command()
@click.option('--stage', default=ALLOWED_STAGES[0], type=click.Choice(ALLOWED_STAGES), prompt='Set stage', help='The release stage.')
@click.option('--version', default=CURRENT_VERSION_TAG, prompt='Set version', help='The version you want to publish. E.g: 4.2.0-SNAPSHOT, 4.2.0-beta.1, or 4.2.0. If you set the version to "%s", the script will default to the current SNAPSHOT version.' % CURRENT_VERSION_TAG)
def release(stage, version):
	# Validate params
	final_stage = validate_stage(stage=stage)
	final_branch = validate_branch(stage=final_stage)
	final_version = validate_version(stage=final_stage, branch=final_branch, version=version)

	# Get user confirmation
	click.echo('\n===== Build information =====')
	click.echo('- Stage: %s' % final_stage)
	click.echo('- Branch: %s' % final_branch)
	click.echo('- Version: %s' % final_version)
	click.confirm('\nDoes it look right?', abort=True)

	# Proceed
	if (final_stage == 'snapshot'):
		publish_snapshot(branch=final_branch, version=final_version)
	elif (final_stage == 'beta'):
		publish_beta(branch=final_branch, version=final_version)
	elif (final_stage == 'final'):
		publish_final(branch=final_branch, version=final_version)

def validate_stage(stage):
	if stage not in ALLOWED_STAGES:
		abort_with_message('Invalid stage: %s' % stage)
	return stage

def validate_branch(stage):
	branch = git_get_current_branch()
	if not branch:
		abort_with_message('The current folder is not a git repository.')
	if branch == 'master' and stage != 'snapshot':
		abort_with_message('You need to swtich to a release branch for a beta or a final release.')
	return branch

def validate_version(stage, branch, version):
	if stage == 'snapshot' and branch == 'master' and version != CURRENT_VERSION_TAG:
		abort_with_message('You cannot specify a custom version if you are building a snapshot from master.')

	if not version or version == CURRENT_VERSION_TAG:
		version = get_current_version(file_path=GRADLE_PROPERTIES_PATH, file_var=GRADLE_TOKEN)

	if stage == 'snapshot':
		if not 'SNAPSHOT' in version:
			abort_with_message('Version should contain the word SNAPSHOT: %s' % version)
	elif stage == 'beta':
		if not ALLOWED_PRE_RELEASE[0] in version:
			abort_with_message('Version should contain the word %s: %s' % (ALLOWED_PRE_RELEASE[0], version))
	elif stage == 'final':
		if not version or 'SNAPSHOT' in version or ALLOWED_PRE_RELEASE[0] in version:
			abort_with_message('Version cannot be empty, or contain the words SNAPSHOT or %s: %s' % (ALLOWED_PRE_RELEASE[0], version))

	return version

def publish_snapshot(branch, version):
	click.echo('Publishing snapshot for branch: %s (version: %s).' % (branch, version))
	if branch != 'master':
		dirty_gradle = update_current_version(file_path=GRADLE_PROPERTIES_PATH, file_var=GRADLE_TOKEN, version=version)
		if dirty_gradle:
			git_add(path=GRADLE_PROPERTIES_PATH)
			git_commit_and_push(branch=branch, version=version)
	do_circleci_request(branch=branch)

def publish_beta(branch, version):
	click.echo('Publishing beta from branch: %s (version: %s).' % (branch, version))
	dirty_gradle = update_current_version(file_path=GRADLE_PROPERTIES_PATH, file_var=GRADLE_TOKEN, version=version)
	if dirty_gradle:
		git_add(path=GRADLE_PROPERTIES_PATH)
		git_commit_and_push(branch=branch, version=version)
	do_circleci_request(branch=branch)

def publish_final(branch, version):
	click.echo('Publishing final release from branch: %s (version: %s).' % (branch, version))
	dirty_gradle = update_current_version(file_path=GRADLE_PROPERTIES_PATH, file_var=GRADLE_TOKEN, version=version)
	if dirty_gradle:
		git_add(path=GRADLE_PROPERTIES_PATH)
	dirty_fabric = update_current_version(file_path=FABRIC_PROPERTIES_PATH, file_var=FABRIC_TOKEN, version=version)
	if dirty_fabric:
		git_add(path=FABRIC_PROPERTIES_PATH)
	if dirty_gradle or dirty_fabric:
		git_commit_and_push(branch=branch, version=version)
	do_circleci_request(branch=branch)

#
# Utils
#

def abort_with_message(message):
	click.echo(message)
	click.get_current_context().abort()

def execute_call(command):
	click.echo('Executing: %s' % command)
	result = subprocess.call(command, shell=True)
	if result != 0:
		abort_with_message('Command failed: %s' % command)

#
# CircleCI
#

def get_circleci_api_token():
	circleci_api_token = os.environ.get(CIRCLECI_API_TOKEN_ENV_VAR)
	if not circleci_api_token:
		abort_with_message('You need to set the CIRCLECI_API_TOKEN environment variable.')
	click.echo('Found CircleCI API token.')
	return circleci_api_token

def do_circleci_request(branch):
	url = URL_CIRCLECI + branch
	params = {'circle-token': get_circleci_api_token()}
	click.echo('CircleCI request to %s (params: %s)' % (url, json.dumps(params)))
	click.confirm('\nDo you want to start a build?', abort=True)

	r = requests.post(url, params=params)
	click.echo('- CircleCI response code: %s' % r.status_code)

#
# Git
#

def git_get_current_branch():
	return subprocess.check_output('git symbolic-ref --short HEAD'.split(' ')).strip()

def git_add(path):
	execute_call(command='git add %s' % path)

def git_commit_and_push(branch, version):
	message = '[android] [auto] Update properties to version %s in preparation for build.' % version
	commands = [
		'git commit -m "%s"' % message,
		'git push -u origin %s' % branch]
	for command in commands:
		execute_call(command=command)

#
# Read and update properties files
#

def get_current_version(file_path, file_var):
	click.echo('Getting current version from %s.' % file_path)
	with open(file_path, 'r') as f:
		for line in f:
			if line.startswith(file_var):
				version_name = line[len(file_var):].strip()
				click.echo('Current version is %s.' % version_name)
				return version_name
	return None

def update_current_version(file_path, file_var, version):
	dirty = False
	click.echo('Updating file to version %s: %s.' % (version, file_path))
	with open(file_path, 'r') as f:
		file_lines = f.readlines()
	for line_number in range(len(file_lines)):
		if file_lines[line_number].startswith(file_var):
			content_old = file_lines[line_number]
			content_new = '%s%s\n' % (file_var, version)
			if (content_old != content_new):
				click.echo('%s -> %s' % (content_old.strip(), content_new.strip()))
				file_lines[line_number] = content_new
				dirty = True
	if dirty:
		with open(file_path, 'w') as f:
			f.writelines(file_lines)
	else:
		click.echo('File already has the right version.')
	return dirty

if __name__ == '__main__':
	release()
