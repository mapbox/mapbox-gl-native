#!/usr/bin/env bash

set -e
set -o pipefail
set -u

SLACK_CHANNEL=${SLACK_CHANNEL:-''}
SLACK_COLOR=${SLACK_COLOR:-''}

curl -g -H "Content-Type: application/json" -X POST \
  -d "{\"channel\": \"$SLACK_CHANNEL\", \"attachments\": [{\"text\": \"$SLACK_MESSAGE\", \"color\": \"$SLACK_COLOR\", \"author_name\": \"$CIRCLE_PROJECT_USERNAME/$CIRCLE_PROJECT_REPONAME\"}]}" \
  $SLACK_WEBHOOK_URL
