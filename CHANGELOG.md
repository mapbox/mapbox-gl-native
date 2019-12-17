# Changelog

## Master

### New features
- [core] Port line-sort-key and fill-sort-key ([#15839](https://github.com/mapbox/mapbox-gl-native/pull/15839))

  The new feature allows to sort line and fill layer features. Similar to `symbol-sort-key`.

- [core] Add image sections to format expression ([#15937](https://github.com/mapbox/mapbox-gl-native/pull/15937))

  The new feature allows to embed images into labels.

- [core] Introduce OfflineDatabase::runPackDatabaseAutomatically() API ([#15967](https://github.com/mapbox/mapbox-gl-native/pull/15967))

  The new API allows to enable / disable automatic packing of a database.

- [core] Decouple offline storage vacuum and the deleteRegion API ([#15899](https://github.com/mapbox/mapbox-gl-native/pull/15899))
  - introduce `OfflineDatabase::pack()` standing for incremental vacuum
  - make packing optional at offline region deletion

- [core] Implement image expression (#15877)

  The [image expression](https://docs.mapbox.com/mapbox-gl-js/style-spec/#expressions-types-image) allows determining an image's availability.

- [core] Add batch conversion of latLngs to/from screenCoords ([#15891](https://github.com/mapbox/mapbox-gl-native/pull/15891))

  This patch introduces batch conversion between LatLng and ScreenCoordinate in Gl-Native core, so for multiple conversions with single point/latLng previously now it can be done with invoking one function call by passing vector of points/latLngs.

### Bug fixes
- [core] Fix really overscaled lines ([#16045](https://github.com/mapbox/mapbox-gl-native/pull/16045))

  We resample lines after sharp corners in some situations. When tiles were really overscaled, sometimes we didn't have enough precision to represent the resampled vertex because it was too close. Disabling this after a certain point prevents this ([#16018](https://github.com/mapbox/mapbox-gl-native/issues/16018)).

- [core] Don't use signed int type for anchor segment ([#16008](https://github.com/mapbox/mapbox-gl-native/pull/16008))

  Erroneous signed to unsigned implicit conversion was used when PlacedSymbol(s) were created in SymbolLayout and signed values were used as indexes in a few other places.

- [core] Increase padding in CollisionIndex for MapMode::Tile ([#15880](https://github.com/mapbox/mapbox-gl-native/pull/15880))

  This increases the padding to 1024 for MapMode::Tile which should be enough to completely include all the data two zoom levels past the data source's max zoom level. Beyond that, 1024 should be enough of a padding to make clipped labels unlikely.

- [core] Blacklist VAO extension for Adreno (TM) 4xx GPUs ([#15978](https://github.com/mapbox/mapbox-gl-native/pull/15978))

  Blacklist in order to avoid crashes in a driver (Android 5.1.1)

- [core] Retain thread pool in GeoJSONSource ([#15992](https://github.com/mapbox/mapbox-gl-native/pull/15992))

  Otherwise, the construction of the `Immutable<Source::Impl>` in background thread might never yeld.

- [core] Fix supercluster lambdas capturing ([#15989](https://github.com/mapbox/mapbox-gl-native/pull/15989))

  Fix wrapping of the parameters in supercluster map/reduce lambdas (Previously, local variables were wrapped by reference).

- [core] fix rendering of dashed lines with round caps ([#15947](https://github.com/mapbox/mapbox-gl-native/pull/15947))

  In [#15862](https://github.com/mapbox/mapbox-gl-native/pull/15862), we introduced individual textures for line dash patterns to eliminate atlas overflows. Unfortunately, this broke dashed lines that had round caps (dashed lines with straight caps still rendered correctly). Line pattern textures for round caps were now using 256×15 pixel textures.

- [core] Fix incorrect resizing of TileCache ([#15943](https://github.com/mapbox/mapbox-gl-native/pull/15943))

- [core] Avoid GeoJSON source flickering on style transitions ([#15907](https://github.com/mapbox/mapbox-gl-native/pull/15907))

  Before this change, all GeoJSON source cleared tile pyramid at the style transition and it caused flickering.

- [core] Use individual textures for line dash patterns ([#15862](https://github.com/mapbox/mapbox-gl-native/pull/15862))

  This moves the LineAtlas from a shared texture that contained SDF dash patterns to use individual textures.

- [core] Fix collisions with icon-text-fit and variable placement ([#15828](https://github.com/mapbox/mapbox-gl-native/pull/15828))

  When deciding the placement position it was only checking whether there were text collisions. Now, if icon-text-fit is used it now checks whether the icon fits before accepting a placement position.

- [core] Fix icon-text-fit ([#15634](https://github.com/mapbox/mapbox-gl-native/pull/15634))

  This fixes rendering by account for the 1px texture padding around icons that were stretched with icon-text-fit.

### Performance improvements
- [core] Calculate GeoJSON tile geometries in a background thread ([#15953](https://github.com/mapbox/mapbox-gl-native/pull/15953))

  Call `mapbox::geojsonvt::GeoJSONVT::getTile()` in a background thread, so that the rendering thread is not blocked.

- [core] Make requests for expired resources lower priority than requests for new resources ([#15950](https://github.com/mapbox/mapbox-gl-native/pull/15950))

- [core] Add Scheduler::scheduleAndReplyValue() API ([#15885](https://github.com/mapbox/mapbox-gl-native/pull/15885))

  This change converts GeoJSON features to tiles for the loaded source description in a background thread and thus unblock the UI thread.

- [core] Enable incremental vacuum for Offline DB ([#15837](https://github.com/mapbox/mapbox-gl-native/pull/15837))

  Thus we avoid re-creating the whole database and keeping the backup file as it happens on calling VACUUM.

- [core] Fix image requests for already obtained images ([#15825](https://github.com/mapbox/mapbox-gl-native/pull/15825))

  Before this fix, repeated request for an already obtained image was erroneously treated as pending, and it prevented from the tiles load completion.

### Architectural changes
- [core] Remove Map::cycleDebugOptions ([#16005](https://github.com/mapbox/mapbox-gl-native/pull/16005))

  This function was mostly used by the Android API, which is no longer necessary.

- [core] Merge style::Layer::set{Layout,Paint}Property ([#15997](https://github.com/mapbox/mapbox-gl-native/pull/15997))

- [core] Use expected.hpp from mapbox-base ([#15898](https://github.com/mapbox/mapbox-gl-native/pull/15898))
