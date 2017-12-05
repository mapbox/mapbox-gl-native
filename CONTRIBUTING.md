# Contributing

If you have a usage question for a product built on Mapbox GL (such as our iOS or Android SDKs), please visit https://www.mapbox.com/help/.

If you want to contribute code:

1. Please familiarize yourself with the [install process](INSTALL.md).

1. Ensure that existing [pull requests](https://github.com/mapbox/mapbox-gl-native/pulls) and [issues](https://github.com/mapbox/mapbox-gl-native/issues) don’t already cover your contribution or question.

1. Pull requests are gladly accepted. If there are any changes that developers using one of the GL SDKs should be aware of, please update the **master** section of the relevant changelog(s):
  * [Mapbox Android SDK](platform/android/CHANGELOG.md)
  * [Mapbox Maps SDK for iOS](platform/ios/CHANGELOG.md)
  * [Mapbox Maps SDK for macOS](platform/macos/CHANGELOG.md)
  * [node-mapbox-gl-native](platform/node/CHANGELOG.md)

1. Prefix your commit messages with the platform(s) your changes affect: `[core]`, `[android]`, `[ios]`, `[macos]`, `[node]`, or `[qt]`.

Please note the special instructions for contributing new source code files, asset files, or user-facing strings to the [iOS SDK](platform/ios/DEVELOPING.md#contributing) or [macOS SDK](platform/macos/DEVELOPING.md#contributing).

### Github issue labels

Our labeling system is:

 * **minimalistic:** Labels’ usefulness are inversely proportional to how many we have.
 * **objective:** Labels should be objective enough that any two people would agree on a labeling decision.
 * **useful:** Labels should track state or capture semantic meaning that would otherwise be hard to search for.

We’ve color-coded our labels by facet to make them easier to use:

 * type (blue)
 * platform (black)
 * actionable status (red)
 * non-actionable status (grey)
 * importance / urgency (green)
 * topic / project / misc (yellow)
