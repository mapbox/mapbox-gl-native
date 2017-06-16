# Changelog for Mapbox Android SDK

Mapbox welcomes participation and contributions from everyone.  If you'd like to do so please see the [`Contributing Guide`](https://github.com/mapbox/mapbox-gl-native/blob/master/CONTRIBUTING.md) first to get started.

## master
* Add support for ImageSource [#9110](https://github.com/mapbox/mapbox-gl-native/pull/9110)

## 5.1.0 - TBA
* Fix tracking mode + camera race condition [#9133](https://github.com/mapbox/mapbox-gl-native/pull/9133)
* Harden orientation changes [#9128](https://github.com/mapbox/mapbox-gl-native/pull/9128)

## 5.1.0-beta.3 - May 26, 2017

* Add binding support for Light [#9013](https://github.com/mapbox/mapbox-gl-native/pull/9013)
* Update attribution with new Mapbox wordmark [#8774](https://github.com/mapbox/mapbox-gl-native/pull/8774)
* LatLngBounds bearing default value [#9102](https://github.com/mapbox/mapbox-gl-native/pull/9102)
* Stop location updates when toggling MyLocationView [#9099](https://github.com/mapbox/mapbox-gl-native/pull/9099)
* Horizontally rotated in snapshot [#9083](https://github.com/mapbox/mapbox-gl-native/pull/9083)
* Disable letter spacing for Arabic text [#9071](https://github.com/mapbox/mapbox-gl-native/pull/9071)
* Correct bearing conversion when animating the map [#9050](https://github.com/mapbox/mapbox-gl-native/pull/9050)
* Don't leak selected markers when removing [#9047](https://github.com/mapbox/mapbox-gl-native/pull/9047)
* Bump tools and support lib version [#9046](https://github.com/mapbox/mapbox-gl-native/pull/9046)
* MarkerView deselect events with OnMarkerViewClickListener [#9047](https://github.com/mapbox/mapbox-gl-native/pull/9047)
* LOST update to v3.0.0 [#9112](https://github.com/mapbox/mapbox-gl-native/pull/9112)
* Convert dp to pixels for meters per pixel at latitude [#9048](https://github.com/mapbox/mapbox-gl-native/pull/9048)

## 5.1.0-beta.2 - May 12, 2017

5.1.0-beta.2 builds further on 5.1.0-beta.1 and adds:

* When a layer is added, reload its source's tiles [#8963](https://github.com/mapbox/mapbox-gl-native/pull/8963)
* Update release script to support CircleCI builds [#8950](https://github.com/mapbox/mapbox-gl-native/pull/8950)
* URL getter on Sources [#8959](https://github.com/mapbox/mapbox-gl-native/pull/8959)
* Build SNAPSHOT from release branch CI configuration [#8958](https://github.com/mapbox/mapbox-gl-native/pull/8958)
* Fix UI test filter in Makefile [#8960](https://github.com/mapbox/mapbox-gl-native/pull/8960)
* Bump & fixup dependencies [#8921](https://github.com/mapbox/mapbox-gl-native/pull/8921)
* Ignore already deleted region [#8920](https://github.com/mapbox/mapbox-gl-native/pull/8920)
* Keep offline observer when timeout occurs [#8919](https://github.com/mapbox/mapbox-gl-native/pull/8919)
* Show error message when no browser installed [#8920](https://github.com/mapbox/mapbox-gl-native/pull/8920)
* Reset observers of removed Sources and Layers [#8900](https://github.com/mapbox/mapbox-gl-native/pull/8900)
* Only build custom layer .so for debug builds [#8885](https://github.com/mapbox/mapbox-gl-native/pull/8885)
* Update localizations [#8883](https://github.com/mapbox/mapbox-gl-native/pull/8883)
* Reset observers of removed Sources and Layers [#8862](https://github.com/mapbox/mapbox-gl-native/pull/8862)
* Remove force style cascade [#8866](https://github.com/mapbox/mapbox-gl-native/pull/8866)
* Update proguard config [#8944](https://github.com/mapbox/mapbox-gl-native/pull/8944)
* Update LOST to 2.3.0-SNAPSHOT [#8872](https://github.com/mapbox/mapbox-gl-native/pull/8872)
* Update logo [#8774](https://github.com/mapbox/mapbox-gl-native/pull/8774)
* Camera change listener v2.0 [#8644](https://github.com/mapbox/mapbox-gl-native/pull/8644)
* Allow filesource url transform reset [#8957](https://github.com/mapbox/mapbox-gl-native/pull/8957)

## 5.1.0-beta.1 - May 2, 2017

5.1.0-beta.1 builds further on 5.0.2 and adds:

* Support for FillExtrusionLayer [#8431](https://github.com/mapbox/mapbox-gl-native/pull/8431)
* Limit Viewport [#8622](https://github.com/mapbox/mapbox-gl-native/pull/8622)
* Transition Properties for Layer attributes [#8509](https://github.com/mapbox/mapbox-gl-native/pull/8509)
* Style wide transition duration and transition offset in milliseconds [#8576](https://github.com/mapbox/mapbox-gl-native/pull/8576)
* Transifex integration, Catalan & Dutch translations [#8556](https://github.com/mapbox/mapbox-gl-native/pull/8556)
* LatLngBounds includes with another bounds [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* LatLngBounds includes takes in account LatLng on the edges (cfr. core) [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* LatLngBounds facility getters/setters for LatLnbg on the edges of the bounds [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* Expose world bounds in LatLngBounds [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* OfflineRegion are validated if the bounds is found in the world bounds, else onError will be invoked [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* Polygon holes [#8557](https://github.com/mapbox/mapbox-gl-native/pull/8557) and [#8722](https://github.com/mapbox/mapbox-gl-native/pull/8722)
* Custom location source [#8710](https://github.com/mapbox/mapbox-gl-native/pull/8710)
* Expose source layer identifier [#8709](https://github.com/mapbox/mapbox-gl-native/pull/8709)
* Ensure surface is created after display and context [#8759](https://github.com/mapbox/mapbox-gl-native/pull/8759)
* Harden telemetry event dispatch [#8767](https://github.com/mapbox/mapbox-gl-native/pull/8767)
* Move LatLngBounds calculation for CameraUpdateFactory to core [#8765](https://github.com/mapbox/mapbox-gl-native/pull/8765)
* Spanish, Lithuanian, and Vietnamese localizations [#8852](https://github.com/mapbox/mapbox-gl-native/pull/8852)
* Warning when updating a non-added annotation [#8832](https://github.com/mapbox/mapbox-gl-native/pull/8832)
* Share location source between components [#8825](https://github.com/mapbox/mapbox-gl-native/pull/8825)
* Test application runtime permissions for pre lollipop devices [#8823](https://github.com/mapbox/mapbox-gl-native/pull/8823)
* Do not call OnMarkerClick listener twice [#8804](https://github.com/mapbox/mapbox-gl-native/pull/8804)
* Rework instrumentation tests [#8793](https://github.com/mapbox/mapbox-gl-native/pull/8793)
* Set first render flag to false when destroying the surface [#8739](https://github.com/mapbox/mapbox-gl-native/pull/8739)
* Post resetting tracking settings to avoid race condition reset [#8738](https://github.com/mapbox/mapbox-gl-native/pull/8738)
* Expose Source Layer identifier [#8709](https://github.com/mapbox/mapbox-gl-native/pull/8709)
* Derived source attribution [#8630](https://github.com/mapbox/mapbox-gl-native/pull/8630)
* Consistent use of duration unit [#8578](https://github.com/mapbox/mapbox-gl-native/pull/8578)
* Swedish localization [#8883](https://github.com/mapbox/mapbox-gl-native/pull/8883)
* Streets v10, Outdoors v10, Satellite Streets v10, Traffic Day v2, Traffic Night v2 [#6301](https://github.com/mapbox/mapbox-gl-native/pull/6301)

## 5.0.2 - April 3, 2017

5.0.2 is a patch release that contains the following changes:

* Binary shader caching [#8604](https://github.com/mapbox/mapbox-gl-native/pull/8604)
* Fix resource transform callback [#8582](https://github.com/mapbox/mapbox-gl-native/pull/8582)
* Restore onTouch behaviour to 4.x version [#8585](https://github.com/mapbox/mapbox-gl-native/pull/8585)
* Restore anchoring after updating MarkerView Icon [#8519](https://github.com/mapbox/mapbox-gl-native/pull/8519)

## 5.0.1 - March 22nd, 2017

5.0.1 is a patch release that addresses a shader precision issue that created a rendering problem on some devices.

* Use `highp` for color attributes [#8385](https://github.com/mapbox/mapbox-gl-native/issues/8385)

## 5.0.0 - March 17th, 2017

5.0.0 final release contains:

* Support for data-driven styles [#7752](https://github.com/mapbox/mapbox-gl-native/pull/7752)
* Additional APIs to manage runtime styling layers [#8195](https://github.com/mapbox/mapbox-gl-native/pull/8195)
* Allow query all features in sources (`querySourceFeatures`) [#5792](https://github.com/mapbox/mapbox-gl-native/issues/5792)
* Improve accessibility TalkBack support by including content description in views [#8230](https://github.com/mapbox/mapbox-gl-native/pull/8230)
* Consistent double tap zoom acceleration [#7514](https://github.com/mapbox/mapbox-gl-native/issues/7514)
* Allow zooming/scaling to use `AnimationOptions` [#8181](https://github.com/mapbox/mapbox-gl-native/pull/8181)
* Cleanup inconsistencies `float` vs `double` [#4445](https://github.com/mapbox/mapbox-gl-native/issues/4445)
* Add `mapbox_` prefix to attributes [#6482](https://github.com/mapbox/mapbox-gl-native/issues/6482)
* Update LOST to 2.x [#6573](https://github.com/mapbox/mapbox-gl-native/issues/6537)
* MAS submodules (`geojson`, `telemetry`) are now smaller to reduce the overall method count [#7642](https://github.com/mapbox/mapbox-gl-native/pull/7642)
* Telemetry Service is now include automatically via Manifest merge to simplify set up [#8338](https://github.com/mapbox/mapbox-gl-native/pull/8338)
* Support for Android Nougat [#5910](5910-move-listener-logic-for-nougat)
  - Move location listening logic to `onStart`/`onStop` activity lifecylce methods
* Removal of `accessToken` on `MapView` and `MapboxMap` [#5621](https://github.com/mapbox/mapbox-gl-native/issues/5621)
* Introduction of new make targets [#5940](https://github.com/mapbox/mapbox-gl-native/issues/5940)
  - Targets for testing, running and using external tools directly from the command line
* Cleanup Gradle files [#6009](https://github.com/mapbox/mapbox-gl-native/issues/6009)
  - Introducing single purpose `.gradle` files
* Checkstyle integration [#7442](https://github.com/mapbox/mapbox-gl-native/pull/7442)
* Transform refactor [#6532](https://github.com/mapbox/mapbox-gl-native/issues/6532)
  - All camera related code is moved to a dedicated transform class cfr. core architecture
* Encapsulate gestures [#6557](https://github.com/mapbox/mapbox-gl-native/issues/6557)
  - All gesture event code is encapsulated in `MapGestureDetector`
  - All key event code is encapsulated in `MapKeyListener`
* Transparent background for `MyLocationView` [#7116](https://github.com/mapbox/mapbox-gl-native/issues/7116)
* Correctly scale accuracy circle for `MyLocationView` [#8255](https://github.com/mapbox/mapbox-gl-native/pull/8255)
* Update documentation on generate sanity tests [#7147](https://github.com/mapbox/mapbox-gl-native/issues/7147)
* Update documentation on style templates [#7157](https://github.com/mapbox/mapbox-gl-native/issues/7157)
* Fix `NullPointerException` during save instance state [#7157](https://github.com/mapbox/mapbox-gl-native/issues/7157)
* Move management style to core [#7275](https://github.com/mapbox/mapbox-gl-native/issues/7275)
* Fix `OnCameraChange` not called for the final position of animation bug [#7350](https://github.com/mapbox/mapbox-gl-native/issues/7350)
* `MapboxAccountManager` refactor [#6534](https://github.com/mapbox/mapbox-gl-native/issues/6534)
  - Consolidating the location where tokens are set
  - Rename to reflect the increased responsibilities introduced in prior releases
* AnnotationManager refactor [#6067](https://github.com/mapbox/mapbox-gl-native/issues/6067)
  - Extracting all business logic related to annotations into a separate class cfr. to core and the iOS codebase
* Gesture handling improvements:
  - Fix missing call to `onFling` when while pinch zooming [#7666](https://github.com/mapbox/mapbox-gl-native/issues/7666)
  - Round tap-zoom gestures to nearest integer [#8027](https://github.com/mapbox/mapbox-gl-native/pull/8027)
* Support for style-wide transition animation duration and delay [#6779](https://github.com/mapbox/mapbox-gl-native/issues/6779)
* Allow callback parameter for style setting [#8262](https://github.com/mapbox/mapbox-gl-native/issues/8262)
* Support for all animated camera changes to configure dismissing tracking modes [#7854](https://github.com/mapbox/mapbox-gl-native/issues/7854)
* Avoid disabling location tracking on all gestures [#7878](https://github.com/mapbox/mapbox-gl-native/issues/7878)
* `LatLng` objects produced by the SDK are wrapped by default [#4522](https://github.com/mapbox/mapbox-gl-native/issues/4522)
* Proguard update for new SDK packages [#8437](https://github.com/mapbox/mapbox-gl-native/pull/8437)
* Javadoc cleanup [#8438](https://github.com/mapbox/mapbox-gl-native/pull/8438)
* MarkerViewClickListener and MapClickListener called simultaneously [#8447](https://github.com/mapbox/mapbox-gl-native/pull/8447)
* Fix LOST v2.0.0 integration [#8445](https://github.com/mapbox/mapbox-gl-native/pull/8445)
* Crash with OnMapChangeListener [#8380](https://github.com/mapbox/mapbox-gl-native/pull/8380)
* Feature query crash [#8374](https://github.com/mapbox/mapbox-gl-native/pull/8374)
* flyTo fix [#8302](https://github.com/mapbox/mapbox-gl-native/pull/8302)
* Don't dispatch MarkerView motion event up to MapView [#8447](https://github.com/mapbox/mapbox-gl-native/pull/8447)

## 5.0.0-beta.3 - March 10th, 2017

5.0.0-beta.3 focuses on addressing stability issues and bug fixes. It also includes:

* Allow query all features in sources (`querySourceFeatures`) [#5792](https://github.com/mapbox/mapbox-gl-native/issues/5792)
* Improve accessibility TalkBack support by including content description in views [#8230](https://github.com/mapbox/mapbox-gl-native/pull/8230)
* Allow zooming/scaling to use `AnimationOptions` [#8181](https://github.com/mapbox/mapbox-gl-native/pull/8181)
* Telemetry Service is now include automatically via Manifest merge to simplify set up [#8338](https://github.com/mapbox/mapbox-gl-native/pull/8338)
* Correctly scale accuracy circle for `MyLocationView` [#8255](https://github.com/mapbox/mapbox-gl-native/pull/8255)
* Round tap-zoom gestures to nearest integer [#8027](https://github.com/mapbox/mapbox-gl-native/pull/8027)
* Allow callback parameter for style setting [#8262](https://github.com/mapbox/mapbox-gl-native/issues/8262)
* Avoid disabling location tracking on all gestures [#7878](https://github.com/mapbox/mapbox-gl-native/issues/7878)

## 5.0.0-beta.2 - March 1st, 2017

5.0.0-beta.2 focuses on addressing stability issues and bug fixes. It also includes:

* Data-driven styling for `{text,icon}-{color,opacity,halo-color,halo-blur,halo-width}` [#7939](https://github.com/mapbox/mapbox-gl-native/pull/7939)
* Additional APIs to manage runtime styling layers [#8195](https://github.com/mapbox/mapbox-gl-native/pull/8195)

## 5.0.0-beta.1 - February 14th, 2017

* Support for data-driven styles [#7752](https://github.com/mapbox/mapbox-gl-native/pull/7752)
* Consistent double tap zoom acceleration [#7514](https://github.com/mapbox/mapbox-gl-native/issues/7514)
* Cleanup inconsistencies `float` vs `double` [#4445](https://github.com/mapbox/mapbox-gl-native/issues/4445)
* Add `mapbox_` prefix to attributes [#6482](https://github.com/mapbox/mapbox-gl-native/issues/6482)
* Update LOST to 2.0.0 [#6573](https://github.com/mapbox/mapbox-gl-native/issues/6537)
* MAS submodules (geojson, telemetry) are now smaller to reduce the overall method count [#7642](https://github.com/mapbox/mapbox-gl-native/pull/7642)
* Support for Android Nougat [#5910](5910-move-listener-logic-for-nougat)
  - Move location listening logic to onStart/onStop Activity lifecylce methods
* Removal of `accessToken` on `MapView` and `MapboxMap` [#5621](https://github.com/mapbox/mapbox-gl-native/issues/5621)
* Introduction of new make targets [#5940](https://github.com/mapbox/mapbox-gl-native/issues/5940)
  - Targets for testing, running and using external tools directly from the command line
* Cleanup Gradle files [#6009](https://github.com/mapbox/mapbox-gl-native/issues/6009)
  - Introducing single purpose `.gradle` files
* Checkstyle integration [#7442](https://github.com/mapbox/mapbox-gl-native/pull/7442)
* Transform refactor [#6532](https://github.com/mapbox/mapbox-gl-native/issues/6532)
  - All camera related code is moved to a dedicated transform class cfr. core architecture
* Encapsulate Gestures [#6557](https://github.com/mapbox/mapbox-gl-native/issues/6557)
  - All gesture event code is encapsulated in `MapGestureDetector`
  - All key event code is encapsulated in `MapKeyListener`
* Transparent background for `MyLocationView` [#7116](https://github.com/mapbox/mapbox-gl-native/issues/7116)
* Update documentation on generate sanity tests [#7147](https://github.com/mapbox/mapbox-gl-native/issues/7147)
* Update documentation on style templates [#7157](https://github.com/mapbox/mapbox-gl-native/issues/7157)
* `NullPointerException` during save instance state [#7157](https://github.com/mapbox/mapbox-gl-native/issues/7157)
* Move management style to core [#7275](https://github.com/mapbox/mapbox-gl-native/issues/7275)
* `OnCameraChange` not called for the final position of animation bug [#7350](https://github.com/mapbox/mapbox-gl-native/issues/7350)
* `MapboxAccountManager` refactor [#6534](https://github.com/mapbox/mapbox-gl-native/issues/6534)
  - Consolidating the location where tokens are set
  - Rename to reflect the increased responsibilities introduced in prior releases
* AnnotationManager refactor [#6067](https://github.com/mapbox/mapbox-gl-native/issues/6067)
  - Extracting all business logic related to annotations into a separate class cfr. to core and the iOS codebase
* Gesture handling bugs
  - Avoid calls to onFling when while pinch zooming [#7666](https://github.com/mapbox/mapbox-gl-native/issues/7666)
* Support for style-wide transition animation duration and delay [#6779](https://github.com/mapbox/mapbox-gl-native/issues/6779)
* Support for all animated camera changes to configure dismissing tracking modes [#7854](https://github.com/mapbox/mapbox-gl-native/issues/7854)
* `LatLng` objects produced by the SDK are wrapped by default for [compatibility](https://developers.google.com/android/reference/com/google/android/gms/maps/model/LatLng) with the Google Maps API on Android [#4522](https://github.com/mapbox/mapbox-gl-native/issues/4522)

## 4.2.2 - January 27, 2017

Mapbox Android 4.2.2 builds further on v4.2.1 and resolves:
* OnFling gesture improvements
 - Improve fling by calculating animation time based on velocity [#7676](https://github.com/mapbox/mapbox-gl-native/pull/7676)
 - Avoid triggering a fling after a scale gesture [#7675](https://github.com/mapbox/mapbox-gl-native/issues/7675)
* Deselect marker on infowindow click [#7784](https://github.com/mapbox/mapbox-gl-native/issues/7784)
* Update build tools to Bitrise supported one [#7729](https://github.com/mapbox/mapbox-gl-native/issues/7729)
* Removal of redudant log statements in native_map_view.cpp [#7801](https://github.com/mapbox/mapbox-gl-native/issues/7801)
* Invalidate markersviews when updating their position [#7794](https://github.com/mapbox/mapbox-gl-native/issues/7794)
* Avoid clustering unclusterable GeoJSON [#7633](https://github.com/mapbox/mapbox-gl-native/pull/7633)
* Prevent null pointer exceptions when receiving connecitivy change events [#6858](https://github.com/mapbox/mapbox-gl-native/issues/6858)

## 4.2.1 - December 22, 2016

Mapbox Android 4.2.1 is a bugfix release build on top of 4.2.0 and resolves:

* Gesture handling bugs
  - Reverse fling when tilted [#7383](https://github.com/mapbox/mapbox-gl-native/pull/7383)
  - Rotation is hard trigger [#7299](https://github.com/mapbox/mapbox-gl-native/pull/7299)
* Null pointer exception on TrackingSettings [#7505](https://github.com/mapbox/mapbox-gl-native/pull/7505)
* Patches to our CI integration [#7363](https://github.com/mapbox/mapbox-gl-native/pull/7363) & [#7385](https://github.com/mapbox/mapbox-gl-native/pull/7385)
* Screen density is now correctly applied for attributes [#7327](https://github.com/mapbox/mapbox-gl-native/pull/7327)
* Usage of LocalBroadCastReceiver [#7511](https://github.com/mapbox/mapbox-gl-native/pull/7511)

## 4.2.0 - December 14, 2016

Mapbox Android 4.2.0 contains all 4.2.0-beta.5 changes and adds:

* Adds additional documentation to APIs
* Resolved issue with marker views occasionally not showing up until a gesture was performed on map [#7239](https://github.com/mapbox/mapbox-gl-native/pull/7239)
* Added horizontal accuracy to location events [#7237](https://github.com/mapbox/mapbox-gl-native/pull/7237)
* Resolved issue with changing visibility of a layer [#7242](https://github.com/mapbox/mapbox-gl-native/pull/7242)
* Proguard improvement and fixes

## 4.2.0-beta.5 - October 25, 2016

Mapbox Android 4.2.0-beta.5 adds no new features, only bug fixes.

## 4.2.0-beta.4 - October 25, 2016

Mapbox Android 4.2.0-beta.4 contains all 4.2.0-beta.3 changes and adds:

* Query rendered features by source [#6516](https://github.com/mapbox/mapbox-gl-native/issues/6516)
* Decreased logging for release builds [#6787](https://github.com/mapbox/mapbox-gl-native/pull/6787)
* setConnected method to manually set a connected flag [#6618](https://github.com/mapbox/mapbox-gl-native/pull/6618)

## 4.2.0-beta.3 - September 21, 2016

Mapbox Android 4.2.0-beta.3 contains all 4.2.0-beta.2 changes and adds:

* Configurable API endpoint [#6309](https://github.com/mapbox/mapbox-gl-native/pull/6309)
* Adjusted logging level for failed OkHttp requests [6356](https://github.com/mapbox/mapbox-gl-native/issues/6356)
* Offline improvements:
  - Fix SQLite crashes by reverting WAL journal mode [#6320](https://github.com/mapbox/mapbox-gl-native/pull/6320)

## 4.2.0-beta.2 - August 25, 2016

Mapbox Android 4.2.0-beta.2 contains all 4.2.0-beta.1 changes and adds:

* Query rendered features [#5869](https://github.com/mapbox/mapbox-gl-native/issues/5869)
* Clustering [#5852](https://github.com/mapbox/mapbox-gl-native/issues/5852)
* Integrated support for [Mapbox Android Services](https://www.mapbox.com/android-sdk/#mapbox_android_services)
* Offline improvements:
  - Enable external storage to host the tiles database [#5589](https://github.com/mapbox/mapbox-gl-native/issues/5589)
  - Stop delivering status updates when offline download is canceled [#5538](https://github.com/mapbox/mapbox-gl-native/issues/5538)
  - Update SQLite schema with WAL journal mode and normal sync [#5796](https://github.com/mapbox/mapbox-gl-native/pull/5796)
  - Check for network connectivity before requesting resources [#6123](https://github.com/mapbox/mapbox-gl-native/pull/6123)

## 4.2.0-beta.1 - August 6, 2016

Mapbox Android 4.2.0 introduces the Runtime Style API and improves performance.

* Runtime Style API ([#5610](https://github.com/mapbox/mapbox-gl-native/issues/5610))
* Earcut ([#2444](https://github.com/mapbox/mapbox-gl-native/pull/2444))
* Map view rendered with `SurfaceView` for improved performance [#5000](https://github.com/mapbox/mapbox-gl-native/issues/5000)

## 4.1.1 - July 19, 2016

Mapbox Android 4.1.1 is a patch release and is recommended to upgrade to it as soon as possible.

* Default Styles Caching Crash ([#5722](https://github.com/mapbox/mapbox-gl-native/issues/5722))

## 4.1.0 - June 29, 2016

Mapbox Android 4.1.0 builds off our ambitious 4.0.0 version with 3 major new features being released.

* View Based Annotations ([#3276](https://github.com/mapbox/mapbox-gl-native/issues/3276))
* UserLocationView Refactor ([#4396](https://github.com/mapbox/mapbox-gl-native/issues/4396))
* MapboxAccountManager ([#5004](https://github.com/mapbox/mapbox-gl-native/issues/5004))

## 4.1.0-beta.3 - June 20, 2016

* New samples:
  * [Location picker](https://github.com/mapbox/mapbox-gl-native/pull/5391)
  * [Animate and rotate multiple markers](https://github.com/mapbox/mapbox-gl-native/issues/5299)
  * [Scaling marker activity](https://github.com/mapbox/mapbox-gl-native/issues/5409)
* Marker improvements:
  * [Expose MarkerView alpha](https://github.com/mapbox/mapbox-gl-native/pull/5329)
  * [Icon should be optional for MarkerView](https://github.com/mapbox/mapbox-gl-native/pull/5328)
  * [Expose an API to enable selection/deselection of markers on a map tap](https://github.com/mapbox/mapbox-gl-native/pull/5312)
  * [Bring selected MarkerView to the front](https://github.com/mapbox/mapbox-gl-native/pull/5294)
* [Make gesture focal point configurable](https://github.com/mapbox/mapbox-gl-native/pull/5332)

## 4.1.0-beta.2 - June 8, 2016

* Dynamically Update InfoWindow ([#5237](https://github.com/mapbox/mapbox-gl-native/issues/5237))
* armeabi ABI Work On armv7 Devices ([#3985](https://github.com/mapbox/mapbox-gl-native/issues/3985))
* Remove Adapter Requirement For MarkerView ([#5214](https://github.com/mapbox/mapbox-gl-native/issues/5214))
* Always Current Version Style URL Constants ([#5193](https://github.com/mapbox/mapbox-gl-native/issues/5193))
* Random NullPointerException On Telemetry ([#5186](https://github.com/mapbox/mapbox-gl-native/issues/5186))

## 4.1.0-beta.1 - May 26, 2016

Mapbox Android 4.1.0 builds off our ambitious 4.0.0 version with 3 major new features being released.  To help us produce the highest quality SDK possible we're releasing an official Beta release first so that everyone has time to explore it and help hardened it before the official 4.1.0 Final Release.

* View Based Annotations ([#3276](https://github.com/mapbox/mapbox-gl-native/issues/3276))
* UserLocationView Refactor ([#4396](https://github.com/mapbox/mapbox-gl-native/issues/4396))
* MapboxAccountManager ([#5004](https://github.com/mapbox/mapbox-gl-native/issues/5004))

## 4.0.1 - May 12, 2016

Mapbox Android 4.0.1 is a patch release to make this bug fix available sooner.

* MapboxMap.removeAnnotations() doesn't remove markers ([#4553](https://github.com/mapbox/mapbox-gl-native/issues/4553))

## 4.0.0 - March 30, 2016

Mapbox Android 4.0.0 contains the following 3 major new features.

* MapboxMap API Change ([#3145](https://github.com/mapbox/mapbox-gl-native/issues/3145))
* Offline Maps ([#3891](https://github.com/mapbox/mapbox-gl-native/issues/3891))
* Telemetry ([#2421](https://github.com/mapbox/mapbox-gl-native/issues/2421))

## 4.0.0-rc.1 - March 25, 2016

* Default Value Bug Fix for MapboxMapOptions ([#4398](https://github.com/mapbox/mapbox-gl-native/issues/4398))
* NullPointerException When Scrolling ([#4424](https://github.com/mapbox/mapbox-gl-native/issues/4424))
* Platform Specific CHANGELOGS ([#4432](https://github.com/mapbox/mapbox-gl-native/issues/4432))
* Introduce LatLng.wrap() ([#4475](https://github.com/mapbox/mapbox-gl-native/issues/4475))

## 4.0.0-beta.2 - March 21, 2016

* arm64 ABI Support ([#3128](https://github.com/mapbox/mapbox-gl-native/issues/3128))
* Unify Offline And Cache Databases ([#4362](https://github.com/mapbox/mapbox-gl-native/issues/4362))
* Offline Database Vacuuming ([#4342](https://github.com/mapbox/mapbox-gl-native/pull/4342))
* Telemetry User Agent Fix ([#4328](https://github.com/mapbox/mapbox-gl-native/issues/4328))
* OnCameraChange Bug Fix ([#4327](https://github.com/mapbox/mapbox-gl-native/issues/4327))
* OnCameraChangeListener vs getCameraPosition ([#4326](https://github.com/mapbox/mapbox-gl-native/issues/4326))

## 4.0.0-beta.1 - March 15, 2016

Mapbox Android 4.0.0 is the most ambitious Android release to date with 3 major new features being released. To help us produce the highest quality SDK possible we're releasing an official Beta release first so that everyone has time to explore it and help hardened it before the official 4.0.0 Final Release.

* MapboxMap API Change ([#3145](https://github.com/mapbox/mapbox-gl-native/issues/3145))
* Offline Maps ([#3891](https://github.com/mapbox/mapbox-gl-native/issues/3891))
* Telemetry ([#2421](https://github.com/mapbox/mapbox-gl-native/issues/2421))

## 3.2.0 - January 28, 2016

* Fixed crash caused by annotation image with non-integer width or height ([#3031](https://github.com/mapbox/mapbox-gl-native/issues/3031))
* Tracking Mode Reverses Bearing Fix ([#3664](https://github.com/mapbox/mapbox-gl-native/issues/3664))
* GPS Extra Rotation Fix ([#3661](https://github.com/mapbox/mapbox-gl-native/issues/3661))
* Added new methods for getting and setting the min and max zoom levels: `getMinZoom`, `setMinZoom`, `getMaxZoom`, `setMaxZoom`. ([#509](https://github.com/mapbox/mapbox-gl-native/issues/509))

## 3.1.0 - January 20, 2016

* Camera API Callback Improvements ([#3412](https://github.com/mapbox/mapbox-gl-native/issues/3412))
* Coordinate Deprecated For LatLng ([#3309](https://github.com/mapbox/mapbox-gl-native/issues/3309))
* Responsive User Dot Location Tracking ([#2049](https://github.com/mapbox/mapbox-gl-native/issues/2049))

## 3.0.0 - December 21, 2015

* Added Camera API ([#3244](https://github.com/mapbox/mapbox-gl-native/issues/3244))
* Custom Layer Support ([#3248](https://github.com/mapbox/mapbox-gl-native/issues/3348))
* Reverse Tilt Gesutre Detection ([#3245](https://github.com/mapbox/mapbox-gl-native/issues/3245))
* Decoupled Location Provider ([#2954](https://github.com/mapbox/mapbox-gl-native/issues/2954))

## 2.3.0 - December 4, 2015

* Added Tilt / Pitch Support ([#2805](https://github.com/mapbox/mapbox-gl-native/issues/2805))
* Keep InfoWindow Open When Panning ([#3121](https://github.com/mapbox/mapbox-gl-native/issues/3121))
* Concurrent Multiple Open InfoWindows ([#3115](https://github.com/mapbox/mapbox-gl-native/issues/3115))
* OkHttp Replace curl ([#2856](https://github.com/mapbox/mapbox-gl-native/issues/2856))
* GPS and Compass Customization Support ([#2858](https://github.com/mapbox/mapbox-gl-native/issues/2858))

## 2.2.0 - October 28, 2015

- New User Dot location graphics ([#2732](https://github.com/mapbox/mapbox-gl-native/issues/2732))
- Custom Drawable Markers ([#2744](https://github.com/mapbox/mapbox-gl-native/issues/2744))
- `MapView.setOnInfoWindowClickListener()` support ([#2448](https://github.com/mapbox/mapbox-gl-native/issues/2448))
- Completed Annotations API ([#1716](https://github.com/mapbox/mapbox-gl-native/issues/1716))
- Satellite Streets Style ([#2739](https://github.com/mapbox/mapbox-gl-native/issues/2739))
- **RESOLVED** Black Screen On Ice Cream Sandwich and Jelly Bean devices ([#2802](https://github.com/mapbox/mapbox-gl-native/issues/2802))


## 2.1.0 - October 21, 2015

- Initial Android release.

Known issues:

- Black Screen On Ice Cream Sandwich and Jelly Bean devices ([#2802](https://github.com/mapbox/mapbox-gl-native/issues/2802))
  - Resolved in 2.2.0
