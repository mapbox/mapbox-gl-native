# Developing on OS X

To create projects, you can run:

- `make xproj`: Creates an Xcode project with a native OS X application for testing changes to mapbox-gl-native on the desktop.
- `make lproj`: Creates an Xcode project with platform-independent handlers for downloads and settings storage. This is what is also being built on Linux.
- `make osx run-osx`: Builds and runs the OS X application on the command line with `xcodebuild`.

Note that you can't have more than one project in Xcode open at a time because the static library project is shared across the OS X, Linux and iOS project.

### Access Tokens

The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). You will be prompted for this access token the first time you launch the demo application.

### Usage

Through the OS X SDK, the demo application supports a variety of standard gestures and keyboard shortcuts. For more details, open Mapbox GL Help from the Help menu.
