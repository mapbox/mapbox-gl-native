# Developing for iOS on OS X

This process gives you a native iOS app for testing changes to mapbox-gl-native
on a simulator or device. Normally production use of the library is via a
precompiled binary linked into the app target: this process instead compiles
Mapbox GL Native from source along with your application, so it can include
latest development and your own modifications.

Developing for iOS implies an OS X host system.

### Build

Run

    make iproj

Which which will create and open an Xcode project which can build the entire library from source, as well as an Objective-C test app.

If you don't have an Apple Developer account, change the destination from "My Mac" to a simulator such as "iPhone 6" before you run and build the app.

### Access Tokens

_The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/)._

Set up the access token by editing the scheme for the application target, then adding an environment variable with the name `MAPBOX_ACCESS_TOKEN`.

![edit scheme](https://cloud.githubusercontent.com/assets/98601/5460702/c4610262-8519-11e4-873a-8597821da468.png)

![setting access token in Xcode scheme](https://cloud.githubusercontent.com/assets/162976/5349358/0a086f00-7f8c-11e4-8433-bdbaccda2b58.png)

### Test

Run

    make itest

To run the included integration tests on the command line.

If you want to run the tests in Xcode instead, first `make ipackage` to create a local static library version, then open `test/ios/ios-tests.xcodeproj`, and lastly `Command + U` on the `Mapbox GL Tests` application target.

### Usage

- Pan to move
- Pinch to zoom
- Use two fingers to rotate
- Double-tap to zoom in one level
- Two-finger single-tap to zoom out one level
- Double-tap, long-pressing the second, then pan up and down to "quick zoom" (iPhone only, meant for one-handed use)
- Use the debug menu to add test annotations, reset position, and toggle debug info.
