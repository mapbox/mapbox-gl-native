# Mapbox GL Native

A library for embedding interactive, customizable vector maps into native applications on multiple platforms. It takes stylesheets that conform to the [Mapbox Style Specification](https://github.com/mapbox/mapbox-gl-style-spec/), applies them to vector tiles that conform to the [Mapbox Vector Tile Specification](https://github.com/mapbox/vector-tile-spec), and renders them using OpenGL. [Mapbox GL JS](https://github.com/mapbox/mapbox-gl-js) is the WebGL-based counterpart, designed for use on the Web.

## The Mapbox GL ecosystem

This repository hosts the cross-platform Mapbox GL Native library, plus convenient SDKs for several platforms. The cross-platform library comes with a [GLFW](https://github.com/glfw/glfw)-based demo application for Ubuntu Linux and macOS. The SDKs target the usual languages on their respective platforms:

| SDK                                     | Languages                          | Build status                             |
| --------------------------------------- | ---------------------------------- | ---------------------------------------- |
| [Mapbox GL Native](INSTALL.md)          | C++14                              | [![Travis](https://travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds) [![Coverage Status](https://coveralls.io/repos/github/mapbox/mapbox-gl-native/badge.svg?branch=master)](https://coveralls.io/github/mapbox/mapbox-gl-native?branch=master) [![FOSSA Status](https://app.fossa.io/api/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native.svg?type=shield)](https://app.fossa.io/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native?ref=badge_shield)|
| [Mapbox Android SDK](platform/android/) | Java                               | [![Bitrise](https://www.bitrise.io/app/79cdcbdc42de4303.svg?token=_InPF8bII6W7J6kFr-L8QQ&branch=master)](https://www.bitrise.io/app/79cdcbdc42de4303) |
| [Mapbox iOS SDK](platform/ios/)         | Objective-C or Swift               | [![Bitrise](https://www.bitrise.io/app/7514e4cf3da2cc57.svg?token=OwqZE5rSBR9MVWNr_lf4sA&branch=master)](https://www.bitrise.io/app/7514e4cf3da2cc57) |
| [Mapbox macOS SDK](platform/macos/)     | Objective-C, Swift, or AppleScript | [![Bitrise](https://www.bitrise.io/app/155ef7da24b38dcd.svg?token=4KSOw_gd6WxTnvGE2rMttg&branch=master)](https://www.bitrise.io/app/155ef7da24b38dcd) |
| [node-mapbox-gl-native](platform/node/) | Node.js                            | [![Linux](https://travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds) [![macOS](https://www.bitrise.io/app/55e3a9bf71202106.svg?token=5qf5ZUcKVN3LDnHhW7rO0w)](https://www.bitrise.io/app/55e3a9bf71202106) |
| [Mapbox Qt SDK](platform/qt)            | C++03                              | [![Travis](https://travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds) [![Bitrise](https://www.bitrise.io/app/96cfbc97e0245c22.svg?token=GxsqIOGPXhn0F23sSVSsYA&branch=master)](https://www.bitrise.io/app/96cfbc97e0245c22) |

Additional Mapbox GL Native–based libraries for **hybrid applications** are developed outside of this repository:

| Toolkit                                  | Android | iOS | Developer   |
| ---------------------------------------- | --------|-----|------------ |
| [React Native](https://github.com/mapbox/react-native-mapbox-gl/) ([npm](https://www.npmjs.com/package/react-native-mapbox-gl)) | :white_check_mark: | :white_check_mark: |  |
| [Apache Cordova](http://plugins.telerik.com/cordova/plugin/mapbox/) ([npm](https://www.npmjs.com/package/cordova-plugin-mapbox)) | :white_check_mark: | :white_check_mark: | Telerik |
| [NativeScript](http://plugins.telerik.com/nativescript/plugin/mapbox/) ([npm](https://www.npmjs.com/package/nativescript-mapbox/)) | :white_check_mark: | :white_check_mark: | Telerik |
| [Xamarin](https://components.xamarin.com/view/mapboxsdk/) | :white_check_mark: | :white_check_mark: | Xamarin |

If your platform or hybrid application framework isn’t listed here, consider embedding [Mapbox GL JS](https://github.com/mapbox/mapbox-gl-js) using the standard Web capabilities on your platform.

## License
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native.svg?type=large)](https://app.fossa.io/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native?ref=badge_large)
