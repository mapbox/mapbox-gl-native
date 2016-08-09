FROM ubuntu:12.04

ENV DEBIAN_FRONTEND noninteractive

# Add other APT sources and keys
ADD *.list /etc/apt/sources.list.d/
ADD *.gpg.key /tmp/
RUN find /tmp/*.gpg.key | xargs -n1 apt-key add

# Recreate Travis CI environment
RUN apt-get update -y
RUN apt-get install -y git-core python-pip curl automake libtool make cmake pkg-config python-pip \
     libc6 libstdc++6 zlib1g-dev libcurl4-openssl-dev libpng-dev libsqlite3-dev \
     xvfb libglu1-mesa-dev libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev xutils-dev \
     mesa-utils libxxf86vm-dev x11proto-xf86vidmode-dev cmake && \
     pip install awscli
