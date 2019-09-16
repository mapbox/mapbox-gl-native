# Mapbox GL Native

A library for embedding interactive, customizable vector maps into native applications on multiple platforms. It
takes stylesheets that conform to the [Mapbox Style Specification](https://github.com/mapbox/mapbox-gl-style-spec/),
applies them to vector tiles that conform to the [Mapbox Vector Tile Specification](https://github.com/mapbox/vector-tile-spec),
and renders them using OpenGL. [Mapbox GL JS](https://github.com/mapbox/mapbox-gl-js) is the WebGL-based counterpart,
designed for use on the Web.

| SDK                                                 | Languages                          | Build status                             |
| --------------------------------------------------- | ---------------------------------- | ---------------------------------------- |
| Mapbox GL Native Core                               | C++14                              | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) [![Coverage Status](https://codecov.io/gh/mapbox/mapbox-gl-native/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/mapbox-gl-native) |
| [Mapbox Maps SDK for Android](../platform/android/) | Java                               | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) |
| [Mapbox Maps SDK for iOS](../platform/ios/)         | Objective-C or Swift               | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) |
| [Mapbox Maps SDK for macOS](../platform/macos/)     | Objective-C, Swift, or AppleScript | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) |
| [node-mapbox-gl-native](../platform/node/)          | Node.js                            | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) |
| [Mapbox Maps SDK for Qt](../platform/qt)            | C++03                              | [![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) [![AppVeyor CI build status](https://ci.appveyor.com/api/projects/status/3q12kbcooc6df8uc?svg=true)](https://ci.appveyor.com/project/Mapbox/mapbox-gl-native) |

[Additional Mapbox GL Native–based libraries](https://wiki.openstreetmap.org/wiki/Mapbox_GL#Libraries) for **hybrid applications** are developed outside of this repository. If your platform or hybrid application framework isn’t listed there, consider embedding [Mapbox GL JS](https://github.com/mapbox/mapbox-gl-js) using the standard Web capabilities on your platform.

## License

Mapbox GL Native is licensed under the [3-Clause BSD license](../LICENSE.md).

## Developing

The `next` directory contains the next generation buildsystem for Mapbox GL Native, based solely on CMake with the
goal of minimizing the use of scripts, increase portability and support building Mapbox GL Native as a subdirectory
of another CMake project. This new buildsystem is also designed to build offline, making use of pre-installed and
vendorized dependencies. When using the build bot docker image, the build should produce the exact same results as
the bots, making it a hermetically sealed build for Linux, Qt and Android.

### Building and running tests

The following command, executed from the root of this repository tree, will build Mapbox GL Native targeting your
host architecture given that you have all the dependencies installed.

```
$ mkdir build && cd build
$ cmake ../next
$ make -j8
$ make test ARGS=-V
```
