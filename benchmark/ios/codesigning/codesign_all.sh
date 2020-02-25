#!/bin/bash

codesign --force --sign $CODESIGNIDENTITY --entitlements codesigning/BenchmarkApp.app.xcent --timestamp=none ./Release-iphoneos/BenchmarkApp.app
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/BenchmarkApp.app/Frameworks/libXCTestSwiftSupport.dylib
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/BenchmarkApp.app/Frameworks/libXCTestBundleInject.dylib
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/BenchmarkApp.app/Frameworks/XCTAutomationSupport.framework
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/BenchmarkApp.app/Frameworks/XCTest.framework
codesign --force --sign $CODESIGNIDENTITY --deep --entitlements codesigning/BenchmarkAppTests.xctest.xcent --timestamp=none ./Release-iphoneos/BenchmarkApp.app/PlugIns/BenchmarkAppTests.xctest
