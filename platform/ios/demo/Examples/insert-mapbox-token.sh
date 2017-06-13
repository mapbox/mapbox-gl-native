token_file=$SRCROOT/mapbox_access_token

# First check the above path, then the user directory.
# Ignore exit codes from `cat`.
token="$(cat $token_file 2> /dev/null)" || token="$(cat ~/.mapbox 2> /dev/null)"

if [ "$token" ]; then
  plutil -replace MGLMapboxAccessToken -string $token "$TARGET_BUILD_DIR/$INFOPLIST_PATH"
else
  echo 'error: Missing Mapbox access token'
  open 'https://www.mapbox.com/studio/account/tokens/'
  echo "error: Get an access token from <https://www.mapbox.com/studio/account/tokens/>, then create a new file at $token_file that contains the access token."
  exit 1
fi
