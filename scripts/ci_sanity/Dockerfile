FROM ubuntu:disco-20190718

ENV DEBIAN_FRONTEND="noninteractive"
ENV ANDROID_HOME="/opt/android"

WORKDIR /src

# Use faster mirrors from Finland
COPY etc/apt/sources.list /etc/apt

RUN set -eu \
    && apt-get update\
    && apt-get dist-upgrade -y

# Base dependencies
RUN set -eu && apt-get install -y \
    clang-format-8 \
    clang-tidy-8 \
    git \
    npm \
    python3-pip

RUN pip3 install cmake_format

# Cleanup
RUN set -eu && apt-get clean
