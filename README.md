# Mapbox GL Native

A library for embedding interactive vector maps into native applications on multiple platforms. It takes stylesheets that conform to the [Mapbox GL Style Specification](https://github.com/mapbox/mapbox-gl-style-spec/), applies them to vector tiles that conform to the [Mapbox Vector Tile Specification](https://github.com/mapbox/vector-tile-spec), and renders them using OpenGL. [Mapbox GL JS](https://github.com/mapbox/mapbox-gl-js) is the WebGL-based counterpart, designed for use on the Web.

## The Mapbox GL ecosystem

This repository hosts the cross-platform Mapbox GL Native library, plus convenient SDKs for several platforms. The cross-platform library comes with a [GLFW](https://github.com/glfw/glfw)-based demo application for Ubuntu Linux and OS X. The SDKs target the usual languages on their respective platforms:

SDK | Languages | Build status
----|-----------|-------------
[Mapbox GL Native](INSTALL.md) | C++14 | [![Travis](https://travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds)
[Mapbox Android SDK](platform/android/) | Java | [![Bitrise](https://www.bitrise.io/app/79cdcbdc42de4303.svg?token=_InPF8bII6W7J6kFr-L8QQ&branch=master)](https://www.bitrise.io/app/79cdcbdc42de4303)
[Mapbox iOS SDK](platform/ios/) | Objective-C or Swift | [![Bitrise](https://www.bitrise.io/app/7514e4cf3da2cc57.svg?token=OwqZE5rSBR9MVWNr_lf4sA&branch=master)](https://www.bitrise.io/app/7514e4cf3da2cc57)
[Mapbox OS X SDK](platform/osx/) | Objective-C or Swift | [![Bitrise](https://www.bitrise.io/app/155ef7da24b38dcd.svg?token=4KSOw_gd6WxTnvGE2rMttg&branch=master)](https://www.bitrise.io/app/155ef7da24b38dcd)
[node-mapbox-gl-native](platform/node/) | Node.js | [![Travis](https://travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds)

Additional Mapbox GL Native–based libraries are developed outside of this repository:

* [React Native Mapbox GL](https://github.com/mapbox/react-native-mapbox-gl) for React Native applications on iOS and Android
* [QMapboxGL](https://github.com/tmpsantos/qmapboxgl) for Qt-based applications
* Telerik’s [Mapbox plugin](http://plugins.telerik.com/cordova/plugin/mapbox) for Apache Cordova applications

If your platform or hybrid application framework isn’t listed here, consider embedding [Mapbox GL JS](https://github.com/mapbox/mapbox-gl-js) using the standard Web capabilities on your platform.
