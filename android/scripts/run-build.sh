#!/usr/bin/env bash

set -e
set -o pipefail

cwd=$(pwd)

region="us-east-1"
region_ami="ami-f0ed9098"
sleep=10
instance_name="android-gl-build-$TRAVIS_REPO_SLUG-$TRAVIS_JOB_NUMBER"
echo $ami_name

NAME=$TRAVIS_JOB_ID

user_data=$(cat ./android/scripts/user_data.sh)

id=$(aws ec2 run-instances \
    --region $region \
    --image-id $region_ami \
    --count 1 \
    --instance-type m3.large \
    --iam-instance-profile Arn="$ANDROID_BUILD_INSTANCE_PROFILE" \
    --instance-initiated-shutdown-behavior terminate \
    --user-data "$user_data" | jq -r '.Instances[0].InstanceId')

echo "Instance: $region $id"

sleep 10

result=$(aws ec2 create-tags --region $region --resources $id --tags "Key=Name,Value=$instance_name")

instance_status_terminated=$(aws ec2 describe-instances --region $region --instance-id $id | jq -r '.Reservations[0].Instances[0].State.Name')

until [ "$instance_status_terminated" = "terminated" ]; do
    instance_status_terminated=$(aws ec2 describe-instances --region $region --instance-id $id | jq -r '.Reservations[0].Instances[0].State.Name')
    echo "Instance terminating status $region $id: $instance_status_terminated"
    sleep $sleep
done

echo "Build finished"

output=$(aws ec2 get-console-output --region $region --instance-id $id | jq -r '.Output')

aws s3 cp --region $region s3://mapbox-gl-testing/android/${NAME}/build-log.txt build.log
cat build.log

if [[ $output == *"ANDROID BUILD PASSED"* ]]; then
    echo "ANDROID BUILD PASSED"
    exit 0
else
    echo "ANDROID BUILD FAILED"
    exit 1
fi
