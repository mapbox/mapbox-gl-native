# Contributing to the Mapbox Maps SDK for iOS

This document explains how to build the Mapbox Maps SDK for iOS from source. It is intended for advanced developers who wish to contribute to Mapbox GL and the Mapbox Maps SDK for iOS.

## Requirements

See the "Requirements" section in [INSTALL.md](INSTALL.md).

## Building the SDK

Create and open an Xcode workspace that includes both the SDK source and some Objective-C test applications by running:

```bash
make iproj
```

### Xcode schemes

Before building, use the scheme picker button in the toolbar to change the scheme to **iosapp** and the destination to one of the simulators or connected devices listed in the menu.

**iosapp** is only one of several shared schemes. Others include:

* **dynamic** builds the SDK as a dynamic framework.
* **static** builds the SDK as a static library and separate resource bundle.
* **dynamic+static** is a combination of the **dynamic** and **static** schemes.
* **CI** builds exactly what gets built by our continuous integration service.
* **bench** is a simple benchmarking application. For more consistent results between runs, run `platform/ios/benchmark/assets/{glyphs,tiles}/download.sh` to download any necessary resources.

If you don’t have an Apple Developer account, change the destination to a simulator before you build and run the app.

### Packaging builds

Install [jazzy](https://github.com/realm/jazzy) for generating API documentation:

```bash
[sudo] gem install jazzy
```

Build and package the SDK by using the `make iframework` command. You can customize the build output by passing the following arguments into the `make` invocation:

* `BUILDTYPE=Release` will optimize for distribution. Defaults to `Debug`.
* `BUILD_DEVICE=false` builds only for the iOS Simulator.
* `FORMAT=dynamic` builds only a dynamic framework. `FORMAT=static` builds only a static framework, for legacy compatibility.
* `SYMBOLS=NO` strips the build output of debug symbols. Defaults to `YES`.

An example command that creates a dynamic framework suitable for eventual App Store distribution:

```bash
make iframework BUILDTYPE=Release
```

The products of these build commands can be found in the `build/ios/pkg` folder at the base of the repository.

## Contributing

### Making any symbol public

To add any Objective-C type, constant, or member to the iOS SDK’s public interface:

1. Ensure that the symbol is pure Objective-C and does not rely on any language features specific to Objective-C++ or the C11 dialect of C – so no namespaced types or classes named with emoji! 🙃 Most projects that depend on this SDK are either written in pure Objective-C (GNU99 dialect) or Swift, which cannot yet bridge C++ types.
1. Name the symbol according to [Cocoa naming conventions](https://developer.apple.com/library/prerelease/content/documentation/Cocoa/Conceptual/CodingGuidelines/CodingGuidelines.html#//apple_ref/doc/uid/10000146i). Use the `MGL` class prefix to avoid conflicts with client code. If the symbol has an analogue in MapKit, name the symbol according to MapKit.
1. Provide full documentation comments. We use [jazzy](https://github.com/realm/jazzy/) to produce the documentation found in the SDK distribution and [on the website for this SDK](https://docs.mapbox.com/ios/api/maps/). We also recognize that many developers rely on Xcode’s Quick Help feature. jazzy supports Markdown formatting; however, Quick Help supports only [HeaderDoc](https://developer.apple.com/legacy/library/documentation/DeveloperTools/Conceptual/HeaderDoc/intro/intro.html) syntax and a subset of Doxygen syntax. For hyperlinks, use HTML syntax, which is recognized by both tools.

### Making a type or constant public

To add an Objective-C class, protocol, category, typedef, enumeration, or global constant to the iOS maps SDK’s public interface:

1. _(Optional.)_ Add the macro `MGL_EXPORT` prior to the declaration for classes and global constants when adding them in shared headers located in `platform/darwin`. To use this macro, include `MGLFoundation.h`. You can check whether all public symbols are exported correctly by running `make darwin-check-public-symbols`.
1. _(Optional.)_ Add the type or constant’s name to the relevant category in the `custom_categories` section of [the jazzy configuration file](./jazzy.yml). This is required for classes and protocols and also recommended for any other type that is strongly associated with a particular class or protocol. If you leave out this step, the symbol will appear in an “Other” section in the generated HTML documentation’s table of contents.
1. _(Optional.)_ If the symbol would also be publicly exposed in the macOS maps SDK, consult [the companion macOS document](../macos/DEVELOPING.md#making-a-type-or-constant-public) for further instructions.

### Adding a source code file

To add an Objective-C header or implementation file to the iOS maps SDK:

1. Add the file to the Headers or Compile Sources build phase, as appropriate, of both the “dynamic” and “static” targets. You can either use the Build Phases tab of the project editor or the Target Membership section of the File inspector.
1. Audit new headers for nullability. Typically, you will wrap a header with `NS_ASSUME_NONNULL_BEGIN` and `NS_ASSUME_NONNULL_END`.
1. _(Optional.)_ If it’s a public header, change its visibility from Project to Public and import it in [the iOS SDK’s umbrella header](./src/Mapbox.h).
1. _(Optional.)_ If the file would also be used by the macOS maps SDK, make sure it’s in [platform/darwin/src/](../darwin/src/), then consult [the companion macOS document](../macos/DEVELOPING.md#adding-a-source-code-file) for further instructions.
1. Run `scripts/generate-file-lists.js` to update the generated source file list for third party build systems.

### Adding a resource

To add a resource (such as an image, SSL certificate, property list, or strings table) to the iOS maps SDK:

1. Add the header to the Copy Bundle Resources build phase of both the “dynamic” and “bundle” targets. You can either use the Build Phases tab of the project editor or the Target Membership section of the File inspector.
1. _(Optional.)_ If the resource would also be used by the macOS maps SDK, make sure it’s in [platform/darwin/resources/](../darwin/resources/), then consult [the companion macOS document](../macos/DEVELOPING.md#adding-a-resource) for further instructions.

### Adding user-facing text

To add or update text that the user may see in the iOS maps SDK:

1. Make sure the implementation file imports [NSBundle+MGLAdditions.h](../darwin/src/NSBundle+MGLAdditions.h).
1. Use the `NSLocalizedStringWithDefaultValue()` macro:
  * `key` is a unique identifier that won’t change if the user-facing text ever needs to change.
  * `tbl` is `Foundation` in code shared between the iOS and macOS maps SDKs, or `nil` otherwise.
  * `bundle` is `nil`; the redefined macro looks for the SDK bundle at runtime and ignores this argument.
  * `val` is the English string.
1. _(Optional.)_ When dealing with a number followed by a pluralized word, do not split the string. Instead, use a format string and make `val` ambiguous, like `%d file(s)`. Then pluralize for English in the appropriate [.stringsdict file](https://developer.apple.com/library/ios/documentation/MacOSX/Conceptual/BPInternational/StringsdictFileFormat/StringsdictFileFormat.html). See [platform/darwin/resources/en.lproj/Foundation.stringsdict](../darwin/resources/en.lproj/Foundation.stringsdict) for an example. Localizers should do likewise for their languages.
1. Run `make genstrings` and commit any changes it makes to .strings files. The make rule also updates the macOS maps SDK’s strings tables.

### Adding a localization

Translations of all the Mapbox GL Native SDKs are managed [in Transifex](https://www.transifex.com/mapbox/mapbox-gl-native/). If your language already has a translation, feel free to complete or proofread it. Otherwise, please [request your language](https://www.transifex.com/mapbox/mapbox-gl-native/languages/). Note that we’re primarily interested in languages that iOS supports as system languages.

Once you’ve finished translating the SDK into a new language in Transifex, perform these steps to make Xcode aware of the translation:

1. In ios.xcworkspace, open the project editor for ios.xcodeproj. Using the project editor’s sidebar or tab bar dropdown, go to the “ios” project; under the Localizations section of the Info tab, click the + button to add your language to the project.
1. In the sheet that appears, select all the .strings and .stringsdict files but not the .storyboard file. (LaunchScreen.storyboard is part of the iosapp example application, which is not localized.) If your language lacks declension and pluralization, as in the case of Chinese, omit the .stringsdict files.
1. In the Project navigator, expand each .stringsdict file in the project. An additional version for your localization should be listed; translate it. See Apple’s documentation on the [.stringsdict format](https://developer.apple.com/library/content/documentation/MacOSX/Conceptual/BPInternational/StringsdictFileFormat/StringsdictFileFormat.html).
1. In the Project navigator, select Demo App/Localizable.strings and then, in the File Inspector, check the box for your new localization.
1. Repeat the steps above in macos.xcworkspace.

The .strings files should still be in the original English – that’s expected. Now you can pull your translations into this repository:

1. _(First time only.)_ Download the [`tx` command line tool](https://docs.transifex.com/client/installing-the-client) and [configure your .transifexrc](https://docs.transifex.com/client/client-configuration).
1. Run `tx pull -a`.
1. Convert any added .strings files from UTF-16 encoding to UTF-8 encoding to facilitate diffing and merging. You can convert the file encoding using Xcode’s File inspector or the following command (substituting _MYLANG_ for the locale code):

```
find platform/{darwin,ios}/resources platform/macos/sdk -path '*/MYLANG.lproj/*.strings' -exec textutil -convert txt -extension strings -inputencoding UTF-16 -encoding UTF-8 {} -output {} \;
```

### Adding a code example

To add an example code listing to the documentation for a class or class member:

 1. Add a test case named in the form `testMGLClass` or `testMGLClass$method` to [MGLDocumentationExamplesTests](test/MGLDocumentationExampleTests.swift).
 2. Wrap the code you'd like to appear in the documentation within the
    following comment blocks:
    ```
    //#-example-code
    ...
    //#-end-example-code
    ```
 3. Insert an empty Swift code block inside the header file where you'd like the
    example code to be inserted.
 4. Run `make darwin-update-examples` to extract example code from the test
    method below and insert it into the header.

[SourceKitten](https://github.com/jpsim/SourceKitten/) is required and will be installed automatically using Homebrew.

### Customizing compilation settings

You can provide an optional and custom [`xcconfig`](https://help.apple.com/xcode/mac/current/#/dev745c5c974) file named `platform/darwin/developer.xcconfig` to set custom build options. This file is ignored by git. These custom settings apply to all configurations (`Debug`, `Release`, `RelWithDebInfo`), but do **not** apply to the core `mbgl` files. This mechanism allows you to try different compiler settings (for example when testing an Xcode beta).

## Testing

`make ios-test` builds and runs unit tests of cross-platform code and of the SDK. Other types of tests available include:

* `make ios-integration-test` runs UI tests from the "Integration Test Harness" scheme.
* `make ios-sanitize` runs unit tests from the "CI" scheme with the Thread Sanitizer and Undefined Behavior Sanitizer enabled.
* `make ios-sanitize-address` runs unit tests from the "CI" scheme with the Address Sanitizer enabled.
* `make ios-static-analyzer` runs unit tests from the "CI" scheme with the Static Analyzer enabled.

These commands are run by default on a single Simulator. To enable legacy iOS versions and more device types, add `MORE_SIMULATORS=YES`. Use `IOS_LATEST=YES`, `IOS_11=YES`, etc. to test on specific iOS versions.

To only run a specific test or class of tests, add `ONLY_TESTING=test/MGLNameOfTestClass/testNameOfTest`.

To skip a specific test or class of tests, add `SKIP_TESTING=test/MGLNameOfTestClass/testNameOfTest`.

To run the cross-platform tests in Xcode instead of on the command line:

1. Run `make iproj` to set up the workspace.
1. Change the scheme to “test (platform project)” and press Command-R to run core unit tests.
1. Change the scheme to “CI” and press Command-U to run SDK integration tests.

## Access tokens

The included applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). 

Before running the demo ("`iosapp`") or benchmarking ("`bench`") applications, first create a new text file called `.mapbox` or `mapbox` in your home directory containing your access token. The access token will then be automatically inserted into the application's Info.plist at runtime.

## Using iosapp

- Pan to move
- Pinch to zoom
- Use two fingers to rotate
- Double-tap to zoom in one level
- Two-finger single-tap to zoom out one level
- Double-tap, long-pressing the second, then pan up and down to "quick zoom" (meant for one-handed use)
- Use the debug menu to add test annotations, reset position, and cycle through the debug options.
