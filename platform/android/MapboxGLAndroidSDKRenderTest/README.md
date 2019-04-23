# Install

Run `yarn install` from the root of this module to pull in external dependencies.

# Generate

Generate tests with `yarn gen`:
 - copies style.json, expected image to `src/main/assets`
 - generates instrumentation tests in `src/androidTest`
 
# Build

Build tests with `yarn build`, this generates 2 apks files that can be uploaded to cloud testing services in `build/outputs/apk`.
 
# Run

Run tests locally on a device with `yarn test`. 

# Retrieve test results

Use `yarn pull` to retrieve test results stored in `sdcard/mapbox/render` from the test device and store them in `build/outputs/render`.

# Pixelmatch

Use `yarn pixelmatch` to run [pixelmatch](https://github.com/mapbox/pixelmatch) on `build/outputs/render` subdirectories.

# Clean

Run `yarn clean` to cleanup build, tests and assets. 