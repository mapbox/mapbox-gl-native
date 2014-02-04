llmr-ios
========

1. Because `libpng` isn't included in the iOS SDK, you will need to build a cross-architecture version yourself. Run `./setup_libpng.sh`, which is derived from Mapnik's cross-architecture build scripts. 

1. `make iapp` to create and build the iOS test app Xcode project. 

1. Consider `sudo npm install -g ios-sim` for auto-launching the simulator, but it can be tricky and it's better to run on an ARM-based device anyway. Open `./llmr-app.xcodeproj`, then build and run on the simulator or a device yourself. 