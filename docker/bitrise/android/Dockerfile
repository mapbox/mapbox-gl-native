FROM bitriseio/android-ndk:latest

# Install Google Cloud SDK for Firebase
RUN set -eu && \
    (echo "deb http://packages.cloud.google.com/apt cloud-sdk-$(lsb_release -c -s) main" | sudo tee /etc/apt/sources.list.d/google-cloud-sdk.list) && \
    (curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -) && \
    sudo apt-get update && \
    sudo apt-get install -y google-cloud-sdk python-dev python-setuptools ccache && \
    sudo apt-get clean && \
    sudo easy_install -U pip && \
    pip install --no-cache-dir awscli && \
    mkdir -p "${ANDROID_HOME}/licenses" && \
    (echo "8933bad161af4178b1185d1a37fbf41ea5269c55" > "${ANDROID_HOME}/licenses/android-sdk-license")

RUN set -eu && \
    git clone --depth=1 "https://github.com/mapbox/mapbox-gl-native.git" . && \
    ccache -z && \
    BUILDTYPE=Debug make android-test-lib-arm-v7 && \
    BUILDTYPE=Debug make android-checkstyle && \
    ccache -s && \
    cd .. && rm -rf src && mkdir src
