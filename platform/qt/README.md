# Mapbox Qt SDK

[![Travis](https://travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds)
[![Bitrise](https://www.bitrise.io/app/96cfbc97e0245c22.svg?token=GxsqIOGPXhn0F23sSVSsYA&branch=master)](https://www.bitrise.io/app/96cfbc97e0245c22)

Provides [Qt](http://www.qt.io/) example applications and APIs via `QMapboxGL`
and `QQuickMapboxGL`:

**QMapboxGL** - implements a C++03x-conformant API that has been tested from Qt
4.7 onwards (Travis CI currently builds it using both Qt 4 and Qt 5).

**QQuickMapboxGL** - implements a Qt Quick (QML) item that can be added to a
scene. Because `QQuickFramebufferObject` has been added in Qt version 5.2, we
support this API from this version onwards.

![Mapbox Qt QML
example](https://c2.staticflickr.com/8/7689/26247088394_01541b34de_o.png)

### Developing

[Qt supports many platforms](https://www.qt.io/qt-framework/#section-4) - so far
we have been testing our code on Linux and macOS desktops, as well as embedded
devices such as Raspberry Pi 3.

If you need to maintain backwards compatibility with prior releases of
Qt, chose `QMapboxGL`. Otherwise, go for `QQuickMapboxGL`.

Both `QMapboxGL` and `QQuickMapboxGL` requires a [Mapbox access
token](https://www.mapbox.com/help/define-access-token/) at runtime - you need
to provide it by setting the environment variable `MAPBOX_ACCESS_TOKEN`:

    export MAPBOX_ACCESS_TOKEN=MYTOKEN

#### Using QMapboxGL

`QMapboxGL` is a [QObject](http://doc.qt.io/qt-5/qobject.html) - [MapWindow](https://github.com/mapbox/mapbox-gl-native/blob/master/platform/qt/app/mapwindow.hpp) provides an example [QGLWidget](http://doc.qt.io/qt-5/qglwidget.html) that contains a `QMapboxGL` object. If you use `QMapboxGL` in non-standard Qt widgets, make sure to initialize the GL extensions required by Mapbox whenever possible:

    QMapbox::initializeGLExtensions();

#### Linux

For Linux (Ubuntu) desktop, together with these [build
instructions](https://github.com/mapbox/mapbox-gl-native/tree/master/platform/linux#build),
you also need:

    sudo apt-get install qt5-default qtlocation5-dev qtpositioning5-dev \
    qml-module-qtquick2 qml-module-qtpositioning qml-module-qtgraphicaleffects \
    qml-module-qtquick-layouts qml-module-qtquick-controls

#### macOS

For macOS desktop, you can install Qt 5 via [Homebrew](http://brew.sh):

    brew install qt5

Since Homebrew doesn't add Qt to the path, you'll have to do that manually before running any Make target:

    export PATH=/usr/local/opt/qt5:$PATH

We provide the following build targets for our Qt SDK:

### Build instructions

#### Qt library

```make qt-lib``` to build the `qmapboxgl` shared library. Public API headers
can be found in [platform/qt/include](https://github.com/mapbox/mapbox-gl-native/tree/master/platform/qt/include) folder.

#### QMapboxGL example application

```make qt-app``` or ```make run-qt-app``` to run the application at the end of
build.

#### QQuickMapboxGL example application:

```make qt-qml-app``` or ```make run-qt-qml-app``` to run the application at the
end of build.
