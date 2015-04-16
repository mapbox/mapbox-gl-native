#!/bin/sh

security delete-keychain ios-build.keychain
rm -f "~/Library/MobileDevice/Provisioning Profiles/$IOS_PROFILE_NAME.mobileprovision"
