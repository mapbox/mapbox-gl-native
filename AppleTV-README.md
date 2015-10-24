# AppleTV

A first run at getting MapboxGL running on AppleTV.

# Discussion

[This discussion on GitHub](https://github.com/mapbox/mapbox-gl-native/pull/2340) is useful for context.

# Building

To build MapboxGL on a Mac you have to install a few dependencies:

* [xcpretty](https://github.com/supermarin/xcpretty); can be installed with ```sudo gem install xcpretty```.
* [appledoc](https://github.com/tomaz/appledoc); can be installed from homebrew with ```brew install appledoc```. Except it doesn't appear to be in homebrew at the moment, so clone, archive, and copy executable into path.

# Status

* Code that doesn't work on tvOS is disabled using preprocessor conditionals for references that don't compile on tvOS.
  * A lot of this related to simplications on the tv. For instance, location doesn't have the concept of speed or heading, because it's a tv.
* asset_request_fs.cpp uses the *nix stat struct to set the modified time on (I assume) asset requests. tvOS doesn't appear to have a mtimespec on this struct, so on tvOS modified is always set to 0. I'm not sure what the implications of this are.
* Build scripts have had tvOS conditions added everywhere I could locate; that part appears to be ok, although the structure may not be optimal.
* Mason has been updated to build tvOS targets; everything builds find without source changes except libuv.
* libuv uses fork and execvp, which are not available on tvOS. Forks of mason and libuv have been made to work around this using preprocessor conditions that just use the error condition, instead of trying to fork.
  * I believe iOS would always fail to fork, so this should result in the same thing. libuv being used seems to be old; this fork extends the latest version.
  * For some reason I had to add a v to a path in the mason script.sh that didn't seem like it should have been necessary.

# Building

```
make tvpackage
```

Find output in **./build/tvos/pkg/static/**

Or ```make tvpackage sim``` if you only want to build for the simulator, for some reason.
