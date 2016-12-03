# Contributing to the Mapbox macOS SDK

This document explains how to build the Mapbox macOS SDK from source. It is intended for advanced developers who wish to contribute to Mapbox GL and the Mapbox iOS SDK.

## Requirements

The Mapbox macOS SDK and the macosapp demo application run on macOS 10.10.0 or above.

The Mapbox macOS SDK requires Xcode 7.3 or above.

## Building the SDK

1. [Install core dependencies](../../INSTALL.md).
1. Run `make xproj`.
1. Switch to the “dynamic” or “macosapp” scheme. The former builds just the Cocoa framework, while the latter also builds a Cocoa demo application based on it.

### Packaging builds

Install [jazzy](https://github.com/realm/jazzy) for generating API documentation:

```bash
[sudo] gem install jazzy
```

Build and package the SDK by using one of the following commands:

* `make xpackage` builds a dynamic framework in the Debug configuration, including debug symbols.
* `make xdocument` generates API documentation using jazzy.

You can customize the build output by passing the following arguments into the `make` invocation:

* `BUILDTYPE=Release` will optimize for distribution. Defaults to `Debug`.
* `SYMBOLS=NO` strips the build output of any debug symbols, yielding much smaller binaries. Defaults to `YES`.

An example command that creates a dynamic framework suitable for distribution:

```bash
make xpackage BUILDTYPE=Release SYMBOLS=NO
```

The products of these build commands can be found in the `build/macos/pkg` folder at the base of the repository.

## Contributing

### Making any symbol public

To add any Objective-C type, constant, or member to the iOS SDK’s public interface:

1. Ensure that the symbol is pure Objective-C and does not rely on any language features specific to Objective-C++ or the C11 dialect of C – so no namespaced types or classes named with emoji! 🙃 Most projects that depend on this SDK are either written in pure Objective-C (GNU99 dialect) or Swift, which cannot yet bridge C++ types.
1. Name the symbol according to [Cocoa naming conventions](https://developer.apple.com/library/prerelease/content/documentation/Cocoa/Conceptual/CodingGuidelines/CodingGuidelines.html#//apple_ref/doc/uid/10000146i). Use the `MGL` class prefix to avoid conflicts with client code. If the symbol has an analogue in MapKit, name the symbol according to MapKit.
1. Provide full documentation comments. We use [jazzy](https://github.com/realm/jazzy/) to produce the documentation found in the SDK distribution. We also recognize that many developers rely on Xcode’s Quick Help feature. jazzy supports Markdown formatting; however, Quick Help supports only [HeaderDoc](https://developer.apple.com/legacy/library/documentation/DeveloperTools/Conceptual/HeaderDoc/intro/intro.html) syntax and a subset of Doxygen syntax. For hyperlinks, use HTML syntax, which is recognized by both tools.

### Making a type or constant public

To add an Objective-C class, protocol, category, typedef, enumeration, or global constant to the macOS SDK’s public interface:

1. _(Optional.)_ Add the type or constant’s name to the relevant category in the `custom_categories` section of [the jazzy configuration file](./jazzy.yml). This is required for classes and protocols and also recommended for any other type that is strongly associated with a particular class or protocol. If you leave out this step, the symbol will appear in an “Other” section in the generated HTML documentation’s table of contents.
1. _(Optional.)_ If the symbol would also be publicly exposed in the iOS SDK, consult [the companion iOS document](../ios/DEVELOPING.md#making-a-type-or-constant-public) for further instructions.

### Adding a source code file

To add an Objective-C header or implementation file to the macOS SDK:

1. Add the file to the “dynamic” target’s Headers or Compile Sources build phase, as appropriate. You can either use the Build Phases tab of the project editor or the Target Membership section of the File inspector.
1. Audit new headers for nullability. Typically, you will wrap a header with `NS_ASSUME_NONNULL_BEGIN` and `NS_ASSUME_NONNULL_END`.
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

### Adding a localization

To add a localization to the macOS SDK:

1. In macos.xcworkspace, open the project editor for macos.xcodeproj. Using the project editor’s sidebar or tab bar dropdown, go to the “macos” project; under the Localizations section of the Info tab, click the + button to add your language to the project.
1. In the sheet that appears, select all the .strings and .stringsdict files but no .xib file. (Most of the XIBs are part of the macosapp example application, which is not localized, while MGLAnnotationCallout.xib contains no localizable strings.)
1. In the Project navigator, expand each .strings and .stringsdict file in the project. An additional version for your localization should be listed; translate it. Translate everything on the right side of the equals sign. Leave the left side and any comments unmodified. See Apple’s documentation on the [.strings](https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/LoadingResources/Strings/Strings.html) and [.stringsdict](https://developer.apple.com/library/content/documentation/MacOSX/Conceptual/BPInternational/StringsdictFileFormat/StringsdictFileFormat.html) formats.
1. You’re already most of the way towards localizing the iOS SDK too – consider [completing that localization](../ios/DEVELOPING.md#adding-a-localization).

## Access tokens

The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). You will be prompted for this access token the first time you launch the demo application.

## Using macosapp

Through the macOS SDK, the demo application supports a variety of standard gestures and keyboard shortcuts. For more details, open Mapbox GL Help from the Help menu.

You can also [integrate the Mapbox macOS SDK into your own Cocoa application](INSTALL.md).
