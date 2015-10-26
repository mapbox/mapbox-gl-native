# Linux

We are using Ubuntu for development. While the software should work on other distributions as well, we are not providing explicit build instructions here.

This process gives you a Linux desktop app built on a Linux host system.

### Build

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

Install glfw3 dependencies:

    sudo apt-get install libxi-dev libglu1-mesa-dev x11proto-randr-dev \
                         x11proto-xext-dev libxrandr-dev \
                         x11proto-xf86vidmode-dev libxxf86vm-dev \
                         libxcursor-dev libxinerama-dev

Set the environment variable `MAPBOX_ACCESS_TOKEN` to your [Mapbox access token](ACCESS_TOKEN.md):

    export MAPBOX_ACCESS_TOKEN=MYTOKEN

Then, you can then proceed to build the library:

    git submodule update --init
    make linux

Set an access token as described below, and then run:

    make run-linux

### Test

- `make test-*` Builds and runs all tests. You can specify individual tests by replacing * with their name.

The `zsh` will treat the * in this command as a glob, so you'll need to run
`make "test-*"` instead.

### Usage

- Press `S` to cycle through bundled styles
- Press `X` to reset the transform
- Press `N` to reset north
- Press `Tab` to toggle debug information
- Press `C` to toggle symbol collision debug boxes
- Press `Esc` to quit
- Press `1` through `6` to add increasing numbers of point annotations for testing
