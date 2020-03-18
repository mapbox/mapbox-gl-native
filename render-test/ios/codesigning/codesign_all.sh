#!/bin/bash
identity=$(/usr/bin/env xcrun security find-identity -v -p codesigning)
var=${identity#*1)\ }
CODE_SIGN_IDENTITY=${var%\ \"Apple\ Development*}

codesign --force --sign $CODE_SIGN_IDENTITY --entitlements codesigning/RenderTestApp.app.xcent --timestamp=none ./Release-iphoneos/RenderTestApp.app
codesign --force --sign $CODE_SIGN_IDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/RenderTestApp.app/Frameworks/libXCTestSwiftSupport.dylib
codesign --force --sign $CODE_SIGN_IDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/RenderTestApp.app/Frameworks/libXCTestBundleInject.dylib
codesign --force --sign $CODE_SIGN_IDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/RenderTestApp.app/Frameworks/XCTAutomationSupport.framework
codesign --force --sign $CODE_SIGN_IDENTITY --deep --preserve-metadata=identifier,entitlements,flags --timestamp=none ./Release-iphoneos/RenderTestApp.app/Frameworks/XCTest.framework
codesign --force --sign $CODE_SIGN_IDENTITY --deep --entitlements codesigning/RenderTestAppTests.xctest.xcent --timestamp=none ./Release-iphoneos/RenderTestApp.app/PlugIns/RenderTestAppTests.xctest
