# Contributing

If you have a usage question for a product built on Mapbox GL (such as our iOS or Android SDKs), please visit https://www.mapbox.com/help/.

If you want to contribute code:

1. Please familiarize yourself with the [install process](INSTALL.md).

1. Ensure that existing [pull requests](https://github.com/mapbox/mapbox-gl-native/pulls) and [issues](https://github.com/mapbox/mapbox-gl-native/issues) don’t already cover your contribution or question.

1. Pull requests are gladly accepted. If there are any changes that developers using one of the GL SDKs should be aware of, please update the **master** section of the relevant changelog(s):
  * [Mapbox Android SDK](platform/android/CHANGELOG.md)
  * [Mapbox iOS SDK](platform/ios/CHANGELOG.md)
  * [Mapbox macOS SDK](platform/macos/CHANGELOG.md)
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

# Code of conduct
Everyone is invited to participate in Mapbox’s open source projects and public discussions: we want to create a welcoming and friendly environment. Harassment of participants or other unethical and unprofessional behavior will not be tolerated in our spaces. The [Contributor Covenant](http://contributor-covenant.org) applies to all projects under the Mapbox organization and we ask that you please read [the full text](http://contributor-covenant.org/version/1/2/0/).

You can learn more about our open source philosophy on [mapbox.com](https://www.mapbox.com/about/open/).
