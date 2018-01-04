#!/usr/bin/env bash
# Created by Cory Dolphin on 03/21/16.
# Copyright (c) 2016 Twitter. All rights reserved.

# Verifies a zip archive submission of SDKs passes basic static checks for format and contents.
# This test is likely to generate false positives, e.g. even if your SDK passes this, you still must
# test integration of your code fully.

# Usage: $ ./validate_zip.sh <path to zip>

set -e

if [ ! -f "$1" ]; then
	printf "No file found at ${1}\n"
	printf "Usage: $ ./validate_zip.sh <path to zip>\n"; exit 1;
fi

function verifyFramework() {
	# set -x
	local FRAMEWORK_PATH=$1
	local FRAMEWORK_NAME=$(basename $FRAMEWORK_PATH)
	local PRODUCT="${FRAMEWORK_NAME%.*}"
	local BINARY_PATH="${FRAMEWORK_PATH}/${PRODUCT}"
	local HEADER_PATH="${FRAMEWORK_PATH}/Headers"
	printf "Found $FRAMEWORK_NAME. Verifying...\n"

	local MODULE_MAP=( $(find $FRAMEWORK_PATH -name "*.modulemap") )
	if [[ -z "$MODULE_MAP" ]]; then
		printf "ERROR: No modulemap found in $FRAMEWORK_NAME\n";
		exit 3;
	fi
	printf "$FRAMEWORK_NAME contains modulemap: ✓\n"

	# Verify there is a modulemap so Swift can use the framework
	if grep -q "link" "$MODULE_MAP"; then
		printf "$FRAMEWORK_NAME modulemap contains dependent system frameworks ✓\n"
	else
		printf "Warning: ${FRAMEWORK_NAME} does not list any system library dependencies. Double check all dependent frameworks and libraries are listed. \n";
	fi

	# Verify there is at least one header listed in the module map
	if grep -q ".*.h" "$MODULE_MAP"; then
		printf "$FRAMEWORK_NAME modulemap contains headers ✓\n";
	else
		printf "Error: ${FRAMEWORK_NAME} does not list any headers in the modulemap\n";
		exit 4;	
	fi

	# Verify there is at least a headers folder
	if [[ ! -d "$HEADER_PATH" ]]; then
		printf "ERROR: Headers not not found in ${FRAMEWORK_NAME}\n";
		exit 5;
	fi

	# Verify the static lib at least has simulator and the two common ARM architectures
	local PRESENT_ARCHITECTURES=$( xcrun lipo -info "${BINARY_PATH}" )
	for arch in "armv7" "arm64" "x86_64"; do
		if [[ ! $PRESENT_ARCHITECTURES == *$arch* ]]; then
			printf "ERROR: Architecture ${arch} not found in ${FRAMEWORK_NAME}\n";
			exit 6;
		fi
	done
	printf "$FRAMEWORK_NAME contains simulator and device architectures: ✓\n"

	# Verify there are at least some bitcode segments in the rmv7 and arm64 slices
	# Note, this is not conclusive, it is possible some symbols are missing the segment
	for arch in "armv7" "arm64"; do
		local SYMBOLS=$(otool -l -arch "${arch}" "${BINARY_PATH}")
 		if [[ ! $SYMBOLS == *"LLVM"* ]]; then
			printf "ERROR: Bitcode segments not found in ${FRAMEWORK_NAME}. Users will fail to archive their builds \n";
			exit 7;
 		fi
	done
	printf "$FRAMEWORK_NAME contains bitcode: ✓\n"

	# Verify there is a plist file
	local PLIST_PATH=( $(find $FRAMEWORK_PATH -name Info.plist) )
	if [[ -z "$PLIST_PATH" ]]; then
		printf "ERROR: No Info.plist found in $FRAMEWORK_NAME\n"
		exit 8;
	fi
	printf "$FRAMEWORK_NAME contains Info.plist: ✓\n"

	# Verify there is a bundle identifier in Info.plist
	# And verify it does not contain any vestigial string templating
	local BUNDLE_NAME=$(/usr/libexec/PlistBuddy -c "Print :CFBundleIdentifier" "${PLIST_PATH}")
	if [[ -z "$BUNDLE_NAME" ]]; then
		printf "ERROR: Info.plist not found in $FRAMEWORK_NAME or CFBundleIdentifier not set\n";
		exit 9;
	elif [[ "$BUNDLE_NAME" == *"$"* ]]; then
		printf "ERROR: CFBundleIdentifier is invalid: $BUNDLE_NAME\n";
		exit 10;
	else
		printf "$FRAMEWORK_NAME has bundle: $BUNDLE_NAME ✓\n"
	fi

	# Verify there is a bundle version in the Info.plist
	local BUNDLE_VERSION=$(/usr/libexec/PlistBuddy -c "Print :CFBundleShortVersionString" "${PLIST_PATH}")
	if [[ -z "$BUNDLE_VERSION" ]]; then
		printf "ERROR: No CFBundleShortVersionString found in $FRAMEWORK_NAME\n";
		exit 11;
	else
		printf "$FRAMEWORK_NAME has version: $BUNDLE_VERSION ✓\n"
	fi

	printf "===========================================\n"
	printf "Analyzed $PRODUCT version $BUNDLE_VERSION. \n"
	printf "Basic static verifications passed. 🚀🚀🚀 \n"
	printf "Please perform final verification testing \n\n"
}

# Extract the zip archive to a temporary location
TEMP_DIR=$(mktemp -d /tmp/fabric_framework_validation.XXXXX)
printf "Unzipping $(basename "$1") to $TEMP_DIR\n"
unzip "$1" -d "$TEMP_DIR" &> /dev/null

# Find frameworks and ensure they are at the top level, e.g. NOT nested within a sub directory
printf "Scanning for frameworks...\n"
FRAMEWORKS=( $(find "$TEMP_DIR" -name "*.framework" -maxdepth 1) )
if [ -z "$FRAMEWORKS" ]; then
	printf "ERROR: No frameworks found at the top level within the zip archive.";
	exit 2;
fi

# Verify each framework found individually
for framework in "${FRAMEWORKS[@]}"; do
	verifyFramework "$framework"
	printf ""
done

rm -r "$TEMP_DIR"
