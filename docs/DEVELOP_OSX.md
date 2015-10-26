# Developing on OS X

To create projects, you can run:

- `make xproj`: Creates an Xcode project with OS X-specific handlers for HTTP downloads and settings storage. It uses [GLFW](http://www.glfw.org) for window handling.
- `make lproj`: Creates an Xcode project with platform-independent handlers for downloads and settings storage. This is what is also being built on Linux.
- `make osx run-osx`: Builds and runs the OS X application on the command line with `xcodebuild`.

Note that you can't have more than one project in Xcode open at a time because the static library project is shared across the OS X, Linux and iOS project.

### Access Tokens

_The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/)._

Set up the access token by editing the scheme for the application target, then adding an environment variable with the name `MAPBOX_ACCESS_TOKEN`.

![edit scheme](https://cloud.githubusercontent.com/assets/98601/5460702/c4610262-8519-11e4-873a-8597821da468.png)

![setting access token in Xcode scheme](https://cloud.githubusercontent.com/assets/162976/5349358/0a086f00-7f8c-11e4-8433-bdbaccda2b58.png)
