# Mapbox GL Native snap for Linux

The `snapcraft.yaml` file in this folder helps build a [snap](https://www.snapcraft.io) package for the Linux demo app (`mbgl-glfw`).

`mbgl-glfw` is a simple map application and test suite for Linux, based on Mapbox GL Native and GLFW. Keyboard shortcuts for testing functionality are logged to the console when the test app is started.

Please note that to run the app you need to set up a `MAPBOX_ACCESS_TOKEN` environment variable with a valid Mapbox access token.

## Install it from the snap store

Builds are available in the `dev` channel of the snap store:

```
sudo snap install mapbox-demo
```

## Building the snap package from source

First increase the default memory available to snapcraft. The default is 2G and won't be enough to build the binary:

```
export SNAPCRAFT_BUILD_ENVIRONMENT_MEMORY=16G
```

After that, run `snapcraft` to build the package. If you're planning to update the Snap Store with this build, make sure you update the version string in `snapcraft.yaml` with the right date:

```
snapcraft --debug
```

(the `--debug` flag will automatically open a shell within the snap's virtual environment if something goes wrong.)

## Installing the snap package

You can install the snap package locally with:

```
sudo snap install mapbox-demo_20190420-master_amd64.snap --dangerous --devmode
```

## Publishing it to the Snap Store

Authenticate the snapcraft command using the Mapbox Snapcraft developer account credentials:

```
snapcraft login
```

Upload the snap and release it into the edge channel:

```
snapcraft push --release=edge mapbox-demo_20190420-master_amd64.snap
```
