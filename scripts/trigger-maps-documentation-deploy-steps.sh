#!/usr/bin/env bash

set -euo pipefail

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

SDK_FLAVOR=${SDK_FLAVOR:-"maps"}

step "Triggering automated site and documentation generation for ${SDK_FLAVOR} SDK ${VERSION_TAG}"

# No branch specified, so default branch will be used
request_body="{
  \"request\": {
    \"message\": \"[${SDK_FLAVOR}] ${VERSION_TAG} automated site and documentation generation\",
    \"config\": {
      \"merge_mode\": \"deep_merge\",
      \"env\": {
        \"SDK_FLAVOR\": \"${SDK_FLAVOR}\",
        \"RELEASE_TAG\": \"${VERSION_TAG}\"
      }
    }
  }
}"

step "Making request…"

# Request URL set to docs-sandbox while testing
curl -s -X POST \
  -H "Content-Type: application/json" \
  -H "Accept: application/json" \
  -H "Travis-API-Version: 3" \
  -H "Authorization: token ${TRAVISCI_API_TOKEN}" \
  -d "${request_body}" \
  https://api.travis-ci.com/repo/mapbox%2F${DOCS_REPO}/requests
