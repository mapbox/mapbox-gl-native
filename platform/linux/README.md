# Linux

A simple map application and test suite for Linux based on [Mapbox GL Native](../../README.md) and [GLFW](https://github.com/glfw/glfw).

We are using Ubuntu for development. While the software should work on other distributions as well, we are not providing explicit build instructions here.

This process gives you a Linux desktop app built on a Linux host system.

### Prerequisites

Install GCC 4.9+ if you are running Ubuntu 14.04 or older. Alternatively, you can also use [Clang 3.5+](http://llvm.org/apt/).

    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install gcc-4.9 g++-4.9
    export CXX=g++-4.9

Ensure you have git and other build essentials:

    sudo apt-get install curl git build-essential zlib1g-dev automake \
                         libtool xutils-dev make cmake pkg-config python-pip \
                         libcurl4-openssl-dev libpng-dev libsqlite3-dev \
                         libllvm3.4

Ensure you have cmake 3.x:

    sudo add-apt-repository --yes ppa:george-edison55/cmake-3.x
    sudo apt-get update
    sudo apt-get install cmake cmake-data

Install glfw3 dependencies:

    sudo apt-get install libxi-dev libglu1-mesa-dev x11proto-randr-dev \
                         x11proto-xext-dev libxrandr-dev \
                         x11proto-xf86vidmode-dev libxxf86vm-dev \
                         libxcursor-dev libxinerama-dev

[Node.js](https://nodejs.org/) 4.2.1 or later is also required.

[ccache](https://ccache.samba.org) is optional, but improves recompilation performance.

## Build

Clone the git repository:

     git clone https://github.com/mapbox/mapbox-gl-native.git
     cd mapbox-gl-native

Note that this repository uses Git submodules. They'll be automatically checked out when you first run a `make` command,
but are not updated automatically. We recommended that you run `git submodule update` after pulling down new commits to
this repository.

Set the environment variable `MAPBOX_ACCESS_TOKEN` to your [Mapbox access token](ACCESS_TOKEN.md):

    export MAPBOX_ACCESS_TOKEN=MYTOKEN

Optionally, you can specify a custom style to the test application:

    export MAPBOX_STYLE_URL=MYSTYLEURL

Then, you can then proceed to build the test application:

    make glfw-app

Set an access token as described below, and then run:

    make run-glfw-app

### Test

- `make test-*` Builds and runs all tests. You can specify individual tests by replacing * with their name (e.g. `make test-Sprite.CustomSpriteImages`).

The `zsh` will treat the * in this command as a glob, so you'll need to run
`make "test-*"` instead.

### Usage

Keyboard shortcuts for testing functionality are logged to the console when the test app is started.
