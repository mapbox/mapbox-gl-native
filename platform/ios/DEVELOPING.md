# Contributing to the Mapbox iOS SDK

This document explains how to build the Mapbox iOS SDK from source. It is intended for advanced developers who wish to contribute to Mapbox GL and the Mapbox iOS SDK.

## Requirements

The Mapbox iOS SDK and iosapp demo application build against the iOS 7.0 SDK. The SDK is intended to run on iOS 7.0 and above, while iosapp is intended to run on iOS 8.0 and above due to the use of a dynamic framework. Both require Xcode on a computer running macOS.

## Building the SDK

Make sure that you have the [core dependencies](../../INSTALL.md) installed.

Create and open an Xcode workspace that includes both the SDK source and some Objective-C test applications by running:

```bash
make iproj
```

### Xcode schemes

Before building, use the scheme picker button in the toolbar to change the scheme to **iosapp** and the destination to one of the simulators or connected devices listed in the menu.

**iosapp** is only one of several shared schemes. Others include:

* **CI** builds exactly what gets built for continuous integration on Bitrise.
* **bench** is a simple benchmarking application. For more consistent results between runs, run `platform/ios/benchmark/assets/{glyphs,tiles}/download.sh` to download any necessary resources.
* **dynamic** builds the SDK as a dynamic framework.
* **static** builds the SDK as a static library and separate resource bundle.
* **dynamic+static** is a combination of the **dynamic** and **static** schemes.

If you don’t have an Apple Developer account, change the destination to a simulator such as “iPhone 6s” before you run and build the app.

### Packaging builds

Install [jazzy](https://github.com/realm/jazzy) for generating API documentation:

```bash
[sudo] gem install jazzy
```

Build and package the SDK by using one of the following commands:

* `make ipackage` builds both dynamic and static frameworks in the Debug configuration for devices and the iOS Simulator.
* `make iframework` builds a dynamic framework in the Debug configuration for devices and the iOS Simulator. The CocoaPods pod downloads the output of this target.
* `make ipackage-sim` builds a dynamic framework in the Debug configuration for the iOS simulator. This is the fastest target.
* `make ipackage-strip` builds both dynamic and static frameworks in the Debug configuration, stripped of debug symbols, for devices and the iOS Simulator.
* `make ifabric` builds a special static framework for compatibility with the Fabric Mac application.

You can customize the build output by passing the following arguments into the `make` invocation:

* `BUILDTYPE=Release` will optimize for distribution. Defaults to `Debug`.
* `BUILD_DEVICE=false` builds only for the iOS Simulator.
* `FORMAT=dynamic` builds only a dynamic framework. `FORMAT=static` builds only a static framework, for compatibility with iOS 7.x.
* `SYMBOLS=NO` strips the build output of any debug symbols, yielding much smaller binaries.

An example command that creates a dynamic framework suitable for eventual App Store distribution:

```bash
make iframework BUILDTYPE=Release SYMBOLS=NO
```

The products of these build commands can be found in the `build/ios/pkg` folder at the base of the repository.

## Contributing

### Making any symbol public

To add any Objective-C type, constant, or member to the iOS SDK’s public interface:

1. Ensure that the symbol is pure Objective-C and does not rely on any language features specific to Objective-C++ or the C11 dialect of C – so no namespaced types or classes named with emoji! 🙃 Most projects that depend on this SDK are either written in pure Objective-C (GNU99 dialect) or Swift, which cannot yet bridge C++ types.
1. Name the symbol according to [Cocoa naming conventions](https://developer.apple.com/library/prerelease/content/documentation/Cocoa/Conceptual/CodingGuidelines/CodingGuidelines.html#//apple_ref/doc/uid/10000146i). Use the `MGL` class prefix to avoid conflicts with client code. If the symbol has an analogue in MapKit, name the symbol according to MapKit.
1. Provide full documentation comments. We use [jazzy](https://github.com/realm/jazzy/) to produce the documentation found in the SDK distribution and [on the Mapbox iOS SDK website](https://www.mapbox.com/ios-sdk/api/). We also recognize that many developers rely on Xcode’s Quick Help feature. jazzy supports Markdown formatting; however, Quick Help supports only [HeaderDoc](https://developer.apple.com/legacy/library/documentation/DeveloperTools/Conceptual/HeaderDoc/intro/intro.html) syntax and a subset of Doxygen syntax. For hyperlinks, use HTML syntax, which is recognized by both tools.

### Making a type or constant public

To add an Objective-C class, protocol, category, typedef, enumeration, or global constant to the iOS SDK’s public interface:

1. _(Optional.)_ Add the type or constant’s name to the relevant category in the `custom_categories` section of [the jazzy configuration file](./jazzy.yml). This is required for classes and protocols and also recommended for any other type that is strongly associated with a particular class or protocol. If you leave out this step, the symbol will appear in an “Other” section in the generated HTML documentation’s table of contents.
1. _(Optional.)_ If the symbol would also be publicly exposed in the macOS SDK, consult [the companion macOS document](../macos/DEVELOPING.md#making-a-type-or-constant-public) for further instructions.

### Adding a source code file

To add an Objective-C header or implementation file to the iOS SDK:

1. Add the file to the Headers or Compile Sources build phase, as appropriate, of both the “dynamic” and “static” targets. You can either use the Build Phases tab of the project editor or the Target Membership section of the File inspector.
1. _(Optional.)_ If it’s a public header, change its visibility from Project to Public and import it in [the iOS SDK’s umbrella header](./src/Mapbox.h).
1. _(Optional.)_ If the file would also be used by the macOS SDK, make sure it’s in [platform/darwin/src/](../darwin/src/), then consult [the companion macOS document](../macos/DEVELOPING.md#adding-a-source-code-file) for further instructions.

### Adding a resource

To add a resource (such as an image, SSL certificate, property list, or strings table) to the iOS SDK:

1. Add the header to the Copy Bundle Resources build phase of both the “dynamic” and “bundle” targets. You can either use the Build Phases tab of the project editor or the Target Membership section of the File inspector.
1. _(Optional.)_ If the resource would also be used by the macOS SDK, make sure it’s in [platform/darwin/resources/](../darwin/resources/), then consult [the companion macOS document](../macos/DEVELOPING.md#adding-a-resource) for further instructions.

### Adding user-facing text

To add or update text that the user may see in the iOS SDK:

1. Make sure the implementation file imports [NSBundle+MGLAdditions.h](../darwin/src/NSBundle+MGLAdditions.h).
1. Use the `NSLocalizedStringWithDefaultValue()` macro:
  * `key` is a unique identifier that won’t change if the user-facing text ever needs to change.
  * `tbl` is `Foundation` in code shared between the iOS and macOS SDKs, or `nil` otherwise.
  * `bundle` is `nil`; the redefined macro looks for the SDK bundle at runtime and ignores this argument.
  * `val` is the English string.
1. _(Optional.)_ When dealing with a number followed by a pluralized word, do not split the string. Instead, use a format string and make `val` ambiguous, like `%d file(s)`. Then pluralize for English in the appropriate [.stringsdict file](https://developer.apple.com/library/ios/documentation/MacOSX/Conceptual/BPInternational/StringsdictFileFormat/StringsdictFileFormat.html). See [platform/darwin/resources/en.lproj/Foundation.stringsdict](../darwin/resources/en.lproj/Foundation.stringsdict) for an example. Localizers should do likewise for their languages.
1. Run `make genstrings` and commit any changes it makes to .strings files. The make rule also updates the macOS SDK’s strings tables.

## Testing

`make ios-test` builds and runs unit tests of cross-platform code as well as the SDK.

Before you can run unit tests of the cross-platform code on the command line, install ios-sim version 3.2.0 (not any other version):

```bash
brew tap mapbox/homebrew-ios-sim-3
brew install mapbox/homebrew-ios-sim-3/ios-sim
```

To instead run the cross-platform tests in Xcode instead of on the command line:

1. Run `make iproj` to set up the workspace.
1. Change the scheme to “test (platform project)” and press Command-R to run core unit tests.
1. Change the scheme to “CI” and press Command-U to run SDK integration tests.

## Access tokens

The included applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). You will be prompted for this access token the first time you launch the demo or benchmarking application. In the iOS Simulator, you can paste your access token by pressing Command-V, holding down the left mouse button in the text field, and pressing the Paste button that appears.

## Using iosapp

- Pan to move
- Pinch to zoom
- Use two fingers to rotate
- Double-tap to zoom in one level
- Two-finger single-tap to zoom out one level
- Double-tap, long-pressing the second, then pan up and down to "quick zoom" (iPhone only, meant for one-handed use)
- Use the debug menu to add test annotations, reset position, and cycle through the debug options.
