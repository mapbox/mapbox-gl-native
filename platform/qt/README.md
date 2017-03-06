# Mapbox Qt SDK

[![Travis](https://travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds)
[![Bitrise](https://www.bitrise.io/app/96cfbc97e0245c22.svg?token=GxsqIOGPXhn0F23sSVSsYA&branch=master)](https://www.bitrise.io/app/96cfbc97e0245c22)

### Developing

This is the foundation for the Mapbox GL plugin available since Qt 5.9. Use the Qt bugtracker
for bugs related to the plugin and this GitHub repository for bugs related to Mapbox GL Native.

#### Linux

For Linux (Ubuntu) desktop, together with these [build
instructions](https://github.com/mapbox/mapbox-gl-native/tree/master/platform/linux#build),
you also need:

    sudo apt-get install qt5-default

#### macOS

For macOS desktop, you can install Qt 5 via [Homebrew](http://brew.sh):

    brew install qt5

Since Homebrew doesn't add Qt to the path, you'll have to do that manually before running any Make target:

    export PATH=/usr/local/opt/qt5/bin:$PATH

We provide the following build targets for our Qt SDK:

### Build instructions

#### Qt library

```make qt-lib``` to build the `qmapboxgl` shared library. Public API headers
can be found in [platform/qt/include](https://github.com/mapbox/mapbox-gl-native/tree/master/platform/qt/include) folder.

#### QMapboxGL example application

```make qt-app``` or ```make run-qt-app``` to run the application at the end of
build.
