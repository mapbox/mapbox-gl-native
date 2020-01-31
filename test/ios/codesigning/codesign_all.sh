#!/bin/bash

codesign --force --sign $CODESIGNIDENTITY --entitlements ../codesigning/UnitTestsApp.app.xcent --timestamp=none ./Release-iphoneos/UnitTestsApp.app
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/UnitTestsApp.app/Frameworks/libXCTestSwiftSupport.dylib
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/UnitTestsApp.app/Frameworks/libXCTestBundleInject.dylib
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/UnitTestsApp.app/Frameworks/XCTAutomationSupport.framework
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/UnitTestsApp.app/Frameworks/XCTest.framework
codesign --force --sign $CODESIGNIDENTITY --deep --entitlements ../codesigning/UnitTestsAppTests.xctest.xcent --timestamp=none ./Release-iphoneos/UnitTestsApp.app/PlugIns/UnitTestsAppTests.xctest
