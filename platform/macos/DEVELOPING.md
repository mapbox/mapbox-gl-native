# Contributing to the Mapbox macOS SDK

This document explains how to build the Mapbox macOS SDK from source. It is intended for advanced developers who wish to contribute to Mapbox GL and the Mapbox iOS SDK.

## Requirements

The Mapbox macOS SDK and the macosapp demo application run on macOS 10.10.0 and above.

## Building the SDK

1. [Install core dependencies](../../INSTALL.md).
1. Run `make xproj`.
1. Switch to the “dynamic” or “macosapp” scheme. The former builds just the Cocoa framework, while the latter also builds a Cocoa demo application based on it.

## Contributing

### Adding a source code file

To add an Objective-C header or implementation file to the macOS SDK:

1. Add the file to the “dynamic” target’s Headers or Compile Sources build phase, as appropriate. You can either use the Build Phases tab of the project editor or the Target Membership section of the File inspector.
1. _(Optional.)_ If it’s a public header, change its visibility from Project to Public and import it in [the macOS SDK’s umbrella header](./src/Mapbox.h).
1. _(Optional.)_ If the file would also be used by the iOS SDK, make sure it’s in [platform/darwin/src/](../darwin/src/), then consult [the companion iOS document](../ios/DEVELOPING.md#adding-a-source-code-file) for further instructions.

### Adding a resource

To add a resource (such as an image, SSL certificate, property list, or strings table) to the macOS SDK:

1. Add the header to the Copy Bundle Resources build phase of the “dynamic” target. You can either use the Build Phases tab of the project editor or the Target Membership section of the File inspector.
1. _(Optional.)_ If the resource would also be used by the iOS SDK, make sure it’s in [platform/darwin/resources/](../darwin/resources/), then consult [the companion iOS document](../ios/DEVELOPING.md#adding-a-resource) for further instructions.

### Adding user-facing text

To add or update text that the user may see in the macOS SDK:

1. Make sure the implementation file imports [NSBundle+MGLAdditions.h](../darwin/src/NSBundle+MGLAdditions.h).
1. Use the `NSLocalizedStringWithDefaultValue()` macro:
  * `key` is a unique identifier that won’t change if the user-facing text ever needs to change.
  * `tbl` is `Foundation` in code shared between the iOS and macOS SDKs, or `nil` otherwise.
  * `bundle` is `nil`; the redefined macro looks for the SDK bundle at runtime and ignores this argument.
  * `val` is the English string.
1. _(Optional.)_ When dealing with a number followed by a pluralized word, do not split the string. Instead, use a format string and make `val` ambiguous, like `%d file(s)`. Then pluralize for English in the appropriate [.stringsdict file](https://developer.apple.com/library/mac/documentation/MacOSX/Conceptual/BPInternational/StringsdictFileFormat/StringsdictFileFormat.html). See [platform/darwin/resources/en.lproj/Foundation.stringsdict](../darwin/resources/en.lproj/Foundation.stringsdict) for an example. Localizers should do likewise for their languages.
1. Run `make genstrings` and commit any changes it makes to .strings files. The make rule also updates the iOS SDK’s strings tables.

## Access tokens

The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). You will be prompted for this access token the first time you launch the demo application.

## Using macosapp

Through the macOS SDK, the demo application supports a variety of standard gestures and keyboard shortcuts. For more details, open Mapbox GL Help from the Help menu.

You can also [integrate the Mapbox macOS SDK into your own Cocoa application](INSTALL.md).
