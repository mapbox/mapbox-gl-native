'''
Utility to schedule SDK builds in Bitrise

TODO:
- Add a wait flag that waits until the release has been built and published.
'''

import click
import json
import os
import requests

# Three stages, from less stable to more stable
ALLOWED_STAGES = ['snapshot', 'beta', 'final']

# Defaults
DEFAULT_BRANCH = 'master'
DEFAULT_VERSION = 'current'

# In the future we might want to consider alpha, or rc.
ALLOWED_PRE_RELEASE = ['beta']

# We get the default version from here
PATH_GRADLE_PROPERTIES = '../MapboxGLAndroidSDK/gradle.properties'

# Bitrise
URL_BITRISE = 'https://www.bitrise.io/app/79cdcbdc42de4303/build/start.json'

# We support three parameters: stage, branch, and version
@click.command()
@click.option('--stage', default=ALLOWED_STAGES[0], type=click.Choice(ALLOWED_STAGES), prompt='Set stage', help='The release stage.')
@click.option('--branch', default=DEFAULT_BRANCH, prompt='Set branch', help='The branch to build from. E.g: master, 1234-fix-crash, or android-release-420.')
@click.option('--version', default=DEFAULT_VERSION, prompt='Set version', help='The version you want to publish. E.g: 4.2.0-SNAPSHOT, 4.2.0-beta.1, or 4.2.0. If you set the version to "%s", the script will default to the current SNAPSHOT version.' % DEFAULT_VERSION)
def release(stage, branch, version):
	# Validate params
	final_stage = validate_stage(stage=stage)
	final_branch = validate_branch(branch=branch)
	final_version = validate_version(stage=final_stage, version=version)

	# Get user confirmation
	click.echo('\nBuild information:')
	click.echo('- Type: %s' % final_stage)
	click.echo('- Branch: %s' % final_branch)
	click.echo('- Version: %s' % final_version)
	click.confirm('Is this correct?', abort=True)

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

def validate_branch(branch):
	if not branch:
		return DEFAULT_BRANCH
	return branch

def validate_version(stage, version):
	if not version or version == DEFAULT_VERSION:
		version = get_current_version()

	if stage == 'snapshot':
		if not 'SNAPSHOT' in version:
			abort_with_message('Version should contain the word SNAPSHOT: %s' % version)
	elif stage == 'beta':
		if not ALLOWED_PRE_RELEASE[0] in version:
			abort_with_message('Version should contain the word %s: %s' % (ALLOWED_PRE_RELEASE[0], version))
	elif stage == 'final':
		if 'SNAPSHOT' in version or ALLOWED_PRE_RELEASE[0] in version:
			abort_with_message('Version cannot contain the words SNAPSHOT or %s: %s' % (ALLOWED_PRE_RELEASE[0], version))

	return version

def publish_snapshot(branch, version):
	click.echo('Publishing snapshot from branch: %s (version: %s).' % (branch, version))

	bitrise_api_token = get_bitrise_api_token()
	data = {
		'hook_info': {'type': 'bitrise', 'api_token':  bitrise_api_token},
		'build_params': {'branch': branch, 'workflow_id': 'scheduled'}}
	print data
	
	# Launch request
	# r = requests.post(URL_BITRISE, data=json.dumps(data))
	# click.echo('Response code: %s' % r.status_code)
	# click.echo('Response content: %s' % r.text)

def publish_beta(branch, version):
	click.echo('Publishing beta from branch: %s (version: %s).' % (branch, version))

def publish_final(branch, version):
	click.echo('Publishing final release from branch: %s (version: %s).' % (branch, version))

#
# Utils
#

def abort_with_message(message):
	click.echo(message)
	click.get_current_context().abort()

def get_bitrise_api_token():
	# Found in https://www.bitrise.io/app/79cdcbdc42de4303#/code -> API token
	bitrise_api_token = os.environ.get('BITRISE_API_TOKEN')
	if not bitrise_api_token:
		abort_with_message('You need to set the BITRISE_API_TOKEN environment variable.')
	return bitrise_api_token

def get_current_version():
	with open(PATH_GRADLE_PROPERTIES, 'r') as f:
		for line in f:
			if line.startswith('VERSION_NAME'):
				version_name = line[len('VERSION_NAME') + 1:]
				return version_name.strip()
	return None

if __name__ == '__main__':
	release()
