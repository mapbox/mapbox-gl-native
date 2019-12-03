# Mapbox GL Native

A library for embedding interactive, customizable vector maps into native applications on multiple platforms. It takes stylesheets that conform to the [Mapbox Style Specification](https://github.com/mapbox/mapbox-gl-style-spec/), applies them to vector tiles that conform to the [Mapbox Vector Tile Specification](https://github.com/mapbox/vector-tile-spec), and renders them using OpenGL. [Mapbox GL JS](https://github.com/mapbox/mapbox-gl-js) is the WebGL-based counterpart, designed for use on the Web.

## The Mapbox GL ecosystem

This repository hosts the cross-platform Mapbox GL Native library, plus convenient SDKs for several platforms. The cross-platform library comes with a [GLFW](https://github.com/glfw/glfw)-based demo application for Ubuntu Linux and macOS. The SDKs target the usual languages on their respective platforms:

| SDK                                     | Languages                          | Build status                             |
| --------------------------------------- | ---------------------------------- | ---------------------------------------- |
| [Mapbox GL Native](INSTALL.md)          | C++14                              | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) [![Coverage Status](https://codecov.io/gh/mapbox/mapbox-gl-native/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/mapbox-gl-native) |
| [Mapbox Maps SDK for Android](https://github.com/mapbox/mapbox-gl-native-android) | Java | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native-android.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native-android/tree/master) |
| [Mapbox Maps SDK for iOS](platform/ios/)         | Objective-C or Swift               | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) |
| [Mapbox Maps SDK for macOS](platform/macos/)     | Objective-C, Swift, or AppleScript | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) |
| [node-mapbox-gl-native](platform/node/) | Node.js                            | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) |
| [Mapbox Maps SDK for Qt](platform/qt)            | C++03                              | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) [![AppVeyor CI build status](https://ci.appveyor.com/api/projects/status/3q12kbcooc6df8uc?svg=true)](https://ci.appveyor.com/project/Mapbox/mapbox-gl-native) |

[Additional Mapbox GL Native–based libraries](https://wiki.openstreetmap.org/wiki/Mapbox_GL#Libraries) for **hybrid applications** are developed outside of this repository. If your platform or hybrid application framework isn’t listed there, consider embedding [Mapbox GL JS](https://github.com/mapbox/mapbox-gl-js) using the standard Web capabilities on your platform.

## License

Mapbox GL Native is licensed under the [2-Clause BSD license](LICENSE.md). The licenses of its dependencies are tracked via [FOSSA](https://app.fossa.io/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native):

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native.svg?type=large)](https://app.fossa.io/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native)
