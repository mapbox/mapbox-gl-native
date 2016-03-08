HEADLESS ?= none
PLATFORM ?= android
ASSET ?= zip
HTTP ?= android
LOOP ?= android

GYP_FLAVOR_SUFFIX=-android

HOST_VERSION ?= arm-v7

ENV = $(shell MASON_ANDROID_ABI=$(HOST_VERSION) ./platform/android/scripts/toolchain.sh)
