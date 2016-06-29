# Changelog for Mapbox Android SDK

Mapbox welcomes participation and contributions from everyone.  If you'd like to do so please see the [`Contributing Guide`](https://github.com/mapbox/mapbox-gl-native/blob/master/CONTRIBUTING.md) first to get started.

## 4.1.0

Mapbox Android 4.1.0 builds off our ambitious 4.0.0 version with 3 major new features being released.

* View Based Annotations ([#3276](https://github.com/mapbox/mapbox-gl-native/issues/3276))
* UserLocationView Refactor ([#4396](https://github.com/mapbox/mapbox-gl-native/issues/4396))
* MapboxAccountManager ([#5004](https://github.com/mapbox/mapbox-gl-native/issues/5004))

## 4.1.0-beta.3

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

## 4.1.0-beta.2

* Dynamically Update InfoWindow ([#5237](https://github.com/mapbox/mapbox-gl-native/issues/5237))
* armeabi ABI Work On armv7 Devices ([#3985](https://github.com/mapbox/mapbox-gl-native/issues/3985))
* Remove Adapter Requirement For MarkerView ([#5214](https://github.com/mapbox/mapbox-gl-native/issues/5214))
* Always Current Version Style URL Constants ([#5193](https://github.com/mapbox/mapbox-gl-native/issues/5193))
* Random NullPointerException On Telemetry ([#5186](https://github.com/mapbox/mapbox-gl-native/issues/5186))

## 4.1.0-beta.1

Mapbox Android 4.1.0 builds off our ambitious 4.0.0 version with 3 major new features being released.  To help us produce the highest quality SDK possible we're releasing an official Beta release first so that everyone has time to explore it and help hardened it before the official 4.1.0 Final Release.

* View Based Annotations ([#3276](https://github.com/mapbox/mapbox-gl-native/issues/3276))
* UserLocationView Refactor ([#4396](https://github.com/mapbox/mapbox-gl-native/issues/4396))
* MapboxAccountManager ([#5004](https://github.com/mapbox/mapbox-gl-native/issues/5004))

## 4.0.1

Mapbox Android 4.0.1 is a patch release to make this bug fix available sooner.

* MapboxMap.removeAnnotations() doesn't remove markers ([#4553](https://github.com/mapbox/mapbox-gl-native/issues/4553))

## 4.0.0

Mapbox Android 4.0.0 contains the following 3 major new features.

* MapboxMap API Change ([#3145](https://github.com/mapbox/mapbox-gl-native/issues/3145))
* Offline Maps ([#3891](https://github.com/mapbox/mapbox-gl-native/issues/3891))
* Telemetry ([#2421](https://github.com/mapbox/mapbox-gl-native/issues/2421))

## 4.0.0-rc.1

* Default Value Bug Fix for MapboxMapOptions ([#4398](https://github.com/mapbox/mapbox-gl-native/issues/4398))
* NullPointerException When Scrolling ([#4424](https://github.com/mapbox/mapbox-gl-native/issues/4424))
* Platform Specific CHANGELOGS ([#4432](https://github.com/mapbox/mapbox-gl-native/issues/4432))
* Introduce LatLng.wrap() ([#4475](https://github.com/mapbox/mapbox-gl-native/issues/4475))

## 4.0.0-beta.2

* arm64 ABI Support ([#3128](https://github.com/mapbox/mapbox-gl-native/issues/3128))
* Unify Offline And Cache Databases ([#4362](https://github.com/mapbox/mapbox-gl-native/issues/4362))
* Offline Database Vacuuming ([#4342](https://github.com/mapbox/mapbox-gl-native/pull/4342))
* Telemetry User Agent Fix ([#4328](https://github.com/mapbox/mapbox-gl-native/issues/4328))
* OnCameraChange Bug Fix ([#4327](https://github.com/mapbox/mapbox-gl-native/issues/4327))
* OnCameraChangeListener vs getCameraPosition ([#4326](https://github.com/mapbox/mapbox-gl-native/issues/4326))

## 4.0.0-beta.1

Mapbox Android 4.0.0 is the most ambitious Android release to date with 3 major new features being released. To help us produce the highest quality SDK possible we're releasing an official Beta release first so that everyone has time to explore it and help hardened it before the official 4.0.0 Final Release.

* MapboxMap API Change ([#3145](https://github.com/mapbox/mapbox-gl-native/issues/3145))
* Offline Maps ([#3891](https://github.com/mapbox/mapbox-gl-native/issues/3891))
* Telemetry ([#2421](https://github.com/mapbox/mapbox-gl-native/issues/2421))

## 3.2.0

* Fixed crash caused by annotation image with non-integer width or height ([#3031](https://github.com/mapbox/mapbox-gl-native/issues/3031))
* Tracking Mode Reverses Bearing Fix ([#3664](https://github.com/mapbox/mapbox-gl-native/issues/3664))
* GPS Extra Rotation Fix ([#3661](https://github.com/mapbox/mapbox-gl-native/issues/3661))
* Added new methods for getting and setting the min and max zoom levels: `getMinZoom`, `setMinZoom`, `getMaxZoom`, `setMaxZoom`. ([#509](https://github.com/mapbox/mapbox-gl-native/issues/509))

## 3.1.0

* Camera API Callback Improvements ([#3412](https://github.com/mapbox/mapbox-gl-native/issues/3412))
* Coordinate Deprecated For LatLng ([#3309](https://github.com/mapbox/mapbox-gl-native/issues/3309))
* Responsive User Dot Location Tracking ([#2049](https://github.com/mapbox/mapbox-gl-native/issues/2049))

## 3.0.0

* Added Camera API ([#3244](https://github.com/mapbox/mapbox-gl-native/issues/3244))
* Custom Layer Support ([#3248](https://github.com/mapbox/mapbox-gl-native/issues/3348))
* Reverse Tilt Gesutre Detection ([#3245](https://github.com/mapbox/mapbox-gl-native/issues/3245))
* Decoupled Location Provider ([#2954](https://github.com/mapbox/mapbox-gl-native/issues/2954))

## 2.3.0

* Added Tilt / Pitch Support ([#2805](https://github.com/mapbox/mapbox-gl-native/issues/2805))
* Keep InfoWindow Open When Panning ([#3121](https://github.com/mapbox/mapbox-gl-native/issues/3121))
* Concurrent Multiple Open InfoWindows ([#3115](https://github.com/mapbox/mapbox-gl-native/issues/3115))
* OkHttp Replace curl ([#2856](https://github.com/mapbox/mapbox-gl-native/issues/2856))
* GPS and Compass Customization Support ([#2858](https://github.com/mapbox/mapbox-gl-native/issues/2858))

## 2.2.0

- New User Dot location graphics ([#2732](https://github.com/mapbox/mapbox-gl-native/issues/2732))
- Custom Drawable Markers ([#2744](https://github.com/mapbox/mapbox-gl-native/issues/2744))
- `MapView.setOnInfoWindowClickListener()` support ([#2448](https://github.com/mapbox/mapbox-gl-native/issues/2448))
- Completed Annotations API ([#1716](https://github.com/mapbox/mapbox-gl-native/issues/1716))
- Satellite Streets Style ([#2739](https://github.com/mapbox/mapbox-gl-native/issues/2739))
- **RESOLVED** Black Screen On Ice Cream Sandwich and Jelly Bean devices ([#2802](https://github.com/mapbox/mapbox-gl-native/issues/2802))


## 2.1.0

- Initial Android release.

Known issues:

- Black Screen On Ice Cream Sandwich and Jelly Bean devices ([#2802](https://github.com/mapbox/mapbox-gl-native/issues/2802))
  - Resolved in 2.2.0
