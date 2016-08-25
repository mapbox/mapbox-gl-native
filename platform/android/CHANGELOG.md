# Changelog for Mapbox Android SDK

Mapbox welcomes participation and contributions from everyone.  If you'd like to do so please see the [`Contributing Guide`](https://github.com/mapbox/mapbox-gl-native/blob/master/CONTRIBUTING.md) first to get started.

## 4.2.0-beta.2 - August 25, 2016

Mapbox Android 4.2.0 introduces the Runtime Style API and improves performance.

* Runtime Style API [#5610](https://github.com/mapbox/mapbox-gl-native/issues/5610)
* Query rendered features [#5869](https://github.com/mapbox/mapbox-gl-native/issues/5869)
* Clustering [#5852](https://github.com/mapbox/mapbox-gl-native/issues/5852)
* Integrated support for [Mapbox Android Services](https://www.mapbox.com/android-sdk/#mapbox_android_services)
* Map view rendered with `SurfaceView` [#5000](https://github.com/mapbox/mapbox-gl-native/issues/5000)
* Earcut [#2444](https://github.com/mapbox/mapbox-gl-native/pull/2444)
* Offline improvements:
  - Enable external storage to host the tiles database [#5589](https://github.com/mapbox/mapbox-gl-native/issues/5589)
  - Stop delivering status updates when offline download is canceled [#5538](https://github.com/mapbox/mapbox-gl-native/issues/5538)
  - Update SQLite schema with WAL journal mode and normal sync [#5796](https://github.com/mapbox/mapbox-gl-native/pull/5796)
  - Check for network connectivity before requesting resources [#6123](https://github.com/mapbox/mapbox-gl-native/pull/6123)

## 4.2.0-beta.1 - August 6, 2016

Mapbox Android 4.2.0 introduces the Runtime Style API and improves performance.

* Runtime Style API ([#5610](https://github.com/mapbox/mapbox-gl-native/issues/5610))
* Earcut ([#2444](https://github.com/mapbox/mapbox-gl-native/pull/2444))
* SurfaceView ([#5000](https://github.com/mapbox/mapbox-gl-native/issues/5000))

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
