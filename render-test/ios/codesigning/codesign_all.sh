#!/bin/bash

codesign --force --sign $CODESIGNIDENTITY --entitlements codesigning/RenderTestApp.app.xcent --timestamp=none ./Release-iphoneos/RenderTestApp.app
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/RenderTestApp.app/Frameworks/libXCTestSwiftSupport.dylib
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/RenderTestApp.app/Frameworks/libXCTestBundleInject.dylib
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/RenderTestApp.app/Frameworks/XCTAutomationSupport.framework
codesign --force --sign $CODESIGNIDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/RenderTestApp.app/Frameworks/XCTest.framework
codesign --force --sign $CODESIGNIDENTITY --deep --entitlements codesigning/RenderTestAppTests.xctest.xcent --timestamp=none ./Release-iphoneos/RenderTestApp.app/PlugIns/RenderTestAppTests.xctest
