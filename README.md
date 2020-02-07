# Mapbox GL Native

[![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master) [![Coverage Status](https://codecov.io/gh/mapbox/mapbox-gl-native/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/mapbox-gl-native)

A library for embedding interactive, customizable vector maps into native applications on multiple platforms. It
takes stylesheets that conform to the [Mapbox Style Specification](https://github.com/mapbox/mapbox-gl-style-spec/),
applies them to vector tiles that conform to the [Mapbox Vector Tile Specification](https://github.com/mapbox/vector-tile-spec),
and renders them using OpenGL or Metal. [Mapbox GL JS](https://github.com/mapbox/mapbox-gl-js) is the WebGL-based counterpart,
designed for use on the Web.

## [Developing](DEVELOPING.md)

We use [CMake](https://cmake.org/cmake/help/latest/) to build Mapbox GL Native
for various platforms, including Linux, Android, iOS, macOS and Windows. The
following command, executed from the root of this repository tree, will build
Mapbox GL Native targeting your host architecture given that you have all the
dependencies installed and run the example app.

```
$ git update submodules --init --recursive
$ cmake . -B build
$ cmake --build build
$ MAPBOX_ACCESS_TOKEN=my_access_token_here ./build/platform/glfw/mbgl-glfw
```

## License

Mapbox GL Native is licensed under the [2-Clause BSD license](LICENSE.md). The licenses of its dependencies are tracked via [FOSSA](https://app.fossa.io/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native):

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native.svg?type=large)](https://app.fossa.io/projects/git%2Bhttps%3A%2F%2Fgithub.com%2Fmapbox%2Fmapbox-gl-native)
