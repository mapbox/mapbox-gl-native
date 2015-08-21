HEADLESS ?= none
PLATFORM ?= android
ASSET ?= zip
HTTP ?= android
CACHE ?= sqlite

GYP_FLAVOR_SUFFIX=-android

HOST_VERSION ?= arm-v7

ENV = $(shell MASON_ANDROID_ABI=$(HOST_VERSION) ./scripts/android/toolchain.sh)
