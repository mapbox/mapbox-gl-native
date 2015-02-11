This is a harness app for integration testing of Mapbox GL Cocoa. It uses the static library build of Mapbox GL Cocoa in order to be entirely self-contained and not need the `mapbox-gl-native` upstream C++ project. 

To run tests, either open the enclosed `ios-tests.xcodeproj` and run the tests or use `xcodebuild test` at the command line from this directory. For example: 

```bash
xcodebuild -scheme 'Mapbox GL Tests' \
           -destination 'platform=iOS Simulator,name=iPad,OS=7.1' \
           test
```
