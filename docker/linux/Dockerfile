FROM ubuntu:12.04

# Recreate Travis CI environment
RUN apt-get update -y && \
    apt-get install -y build-essential git-core python-pip python-software-properties software-properties-common curl zlib1g-dev automake libtool make cmake pkg-config python-pip  libcurl4-openssl-dev libpng-dev libsqlite3-dev xvfb libglu1-mesa-dev libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev imagemagick && \
    pip install awscli

# Install
RUN add-apt-repository --yes ppa:ubuntu-toolchain-r/test && \
    apt-get update -y && \
    apt-get -y install gdb g++-4.9 gcc-4.9 libllvm3.4 xutils-dev libxxf86vm-dev x11proto-xf86vidmode-dev mesa-utils

RUN useradd -ms /bin/bash mapbox
USER mapbox
ENV HOME /home/mapbox
WORKDIR /home/mapbox

# Node
RUN git clone https://github.com/creationix/nvm.git ~/.nvm && \
    . ~/.nvm/nvm.sh && \
    NVM_DIR=~/.nvm nvm install 0.10
