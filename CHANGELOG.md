# Changelog

## master

### 🐞 Bug fixes

- [core] Fix iterators in addRegularDash() ([#16249](https://github.com/mapbox/mapbox-gl-native/pull/16249))

  Fixes possible crashes when using styles with line patterns.

## maps-v1.3.0 (2020.02-relvanillashake)

### 🐞 Bug fixes

- [core] Fix offline region download freezing ([#16230](https://github.com/mapbox/mapbox-gl-native/pull/16230))

  Downloaded resources are put in the buffer and inserted in the database in batches.

  Before this change, the buffer was flushed only at the network response callback and thus it never got flushed if the last required resources were present locally and did not initiate network requests - it caused freezing.

  Now the buffer is flushed every time the remaining resources container gets empty.

### ✨ New features

- [core] Add Layer::serialize() method ([#16231](https://github.com/mapbox/mapbox-gl-native/pull/16231))

  New method allows serialization of a layer into a Value type, including all modifications done via runtime style API. New method is also an enabler for Style object serialization (sources, layers, etc).

- [android] Add jni binding for min and max pitch ([#16236](https://github.com/mapbox/mapbox-gl-native/pull/16236))

- [offline] Offline tool does not hang on 404 error ([#16240](https://github.com/mapbox/mapbox-gl-native/pull/16240))

  The missing resource gets skipped and teh offline region download continues.

##### ⚠️  Breaking changes

- Changes to `mbgl::FileSourceManager::getFileSource()` ([#16238](https://github.com/mapbox/mapbox-gl-native/pull/16238))

  It returns now `mbgl::PassRefPtr<FileSource>` (previously was `std::shared_ptr<FileSource>`) in order to enforce keeping the strong reference to the returned object.

  Breaking code example:
  `auto fs = FileSourceManager::getFileSource(); fs->..`
  
  Posible fix:
  `std::shared_ptr<FileSource> fs = `;

- The `mbgl::OnlineFileSource` class cannot be used directly ([#16238](https://github.com/mapbox/mapbox-gl-native/pull/16238))

  Clients must use the parent `mbgl::FileSource` interface instead.

  Breaking code example:
  `std::shared_ptr<OnlineFileSource> onlineSource = std::static_pointer_cast<OnlineFileSource>(FileSourceManager::get()->getFileSource(..));`

  Possible fix:
  `std::shared_ptr<FileSource> onlineSource = FileSourceManager::get()->getFileSource(..);`

## maps-v1.2.1 (2020.02-release-vanillashake)

### 🐞 Bug fixes

- [core] Revert shader changes from [#16189](https://github.com/mapbox/mapbox-gl-native/pull/16189), which will cause incorrect label positioning ([#16235](https://github.com/mapbox/mapbox-gl-native/pull/16235))

  This is a back porting from GL JS [#9333](https://github.com/mapbox/mapbox-gl-js/pull/9333)

- [ios] Restored support for iOS 9–11 by default ([#16241](https://github.com/mapbox/mapbox-gl-native/pull/16242))

## maps-v1.2.0 (2020.02-release-vanillashake)

### ✨ New features

- [core] Global settings API ([#16174](https://github.com/mapbox/mapbox-gl-native/pull/16174))

  Global settings API provides means of managing non-persistent in-process settings. Initial implementation contains support for experimental option for setting thread priorities.

- Expose READ_ONLY_MODE_KEY property for DatabaseFileSource ([#16183](https://github.com/mapbox/mapbox-gl-native/pull/16183))

  The `READ_ONLY_MODE_KEY` property is exposed for `DatabaseFileSource`.

  This property allows to re-open the offline database in read-only mode and thus improves the performance for the set-ups that do not require the offline database modifications.

- [core] Add runtime API for setting tile prefetch delta for a Source ([#16179](https://github.com/mapbox/mapbox-gl-native/pull/16179))

  The new `Source::setPrefetchZoomDelta(optional<uint8_t>)` method allows overriding default tile prefetch setting that is defined by the Map instance.

- [core] Add support for `within expression`. Implement the use of `within expression` with paint propery and filter expression. ([#16157](https://github.com/mapbox/mapbox-gl-native/pull/16157))
   
  The `within expression` enables checking whether a feature is inside a pre-defined geometry set/boundary or not. This `within expression` returns a boolean value, `true` indicates that the feature being evaluated is inside the geometry set. The returned value can be then consumed as input by another expression or used directly by a paint/layer property.
 
  Support for using `within expression` with layout property will be implemented separately.

- [core] Add support for using `within expression` with layout property. ([#16194](https://github.com/mapbox/mapbox-gl-native/pull/16194))

- [core] Add support for `in expression`. ([#16162](https://github.com/mapbox/mapbox-gl-native/pull/16162))
   
  The `in expression` enables checking whether a Number/String/Boolean type item is in a String/Array and returns a boolean value.

- [core] Add support for using `within expression` with features having `'LineString'` geometry type. ([#16220](https://github.com/mapbox/mapbox-gl-native/pull/16220))
  
  `within expression` now supports features with geometry types: `'Point'` or `'LineString'`.

### 🐞 Bug fixes

- [core] Don't provide multiple responses with the same data for 304 replies ([#16200](https://github.com/mapbox/mapbox-gl-native/pull/16200))

  In cases when cached resource is useable, yet don't have an expiration timestamp, we provided data to the requester from the cache and the same data was returned once 304 response was received from the network.

- [core] Fix potential visual artifact for line-dasharray ([#16202](https://github.com/mapbox/mapbox-gl-native/pull/16202))

- Store gfx::DrawScope objects with associated render objects. ([#15395](https://github.com/mapbox/mapbox-gl-native/pull/15395))

  We used some shared SegmentVectors, e.g. for drawing raster or background tiles. In longer running maps, this lead to resource accumulation. By storing the SegmentVectors and the contained gfx::DrawScope objects, we ensure that resources get released when the associated render objects vanish.

- [core] Fix sprite sheet merging in Style::Impl::onSpriteLoaded ([#16211](https://github.com/mapbox/mapbox-gl-native/pull/16211))

  If we get a new sprite sheet from the server, we need to merge current sprite sheet with a new one, while overwriting duplicates and keeping old unique images in a style.

### 🏁 Performance improvements

 - [core] Loading images to style optimization ([#16187](https://github.com/mapbox/mapbox-gl-native/pull/16187))

   This change enables attaching images to the style with batches and avoids massive re-allocations. Thus, it improves UI performance especially at start-up time.

### 🧩  Architectural changes

##### ⚠️  Breaking changes

 - [core] Loading images to style optimization ([#16187](https://github.com/mapbox/mapbox-gl-native/pull/16187))

   The `style::Style::getImage()` semantics changed - it now returns `optional<style::Image>`.

## maps-v1.0.1 (2020.01-release-unicorn)

### 🐞 Bug fixes

- [core] Use std::list instead of std::map for factory instance ([#16161](https://github.com/mapbox/mapbox-gl-native/pull/16161))

  Factory 'get' method can be invoked recursively and stable iterators are required to guarantee safety.

- [tile mode] Improvements in symbol placement on tile borders ([#16159](https://github.com/mapbox/mapbox-gl-native/pull/16159))

  In tile mode, the placement order of two symbols crossing a tile border is defined by their anchor Y values.

  Symbols crossing the borders between two neighboring tiles are placed with priority.

  It improves symbol placement stability in the tile map mode.

## maps-v1.0.0 (2020.01-release-unicorn)

### ✨ New features

- [core] Implement stretchable icons ([#16030](https://github.com/mapbox/mapbox-gl-native/pull/16030))

  [Stretchable icons](https://github.com/mapbox/mapbox-gl-js/issues/8917) allow defining certain areas of an icon that may be stretched, leaving the remaining areas of an icon at a fixed size.

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

### 🐞 Bug fixes
- [tile mode] Fix variable symbols placement ([#16141](https://github.com/mapbox/mapbox-gl-native/pull/16141)

  This change allows the variable symbols to cross the tile border only if their anchor is the first anchor from the `text-variable-anchor` list.

- [core] Use weak scheduler inside mailbox ([#16136](https://github.com/mapbox/mapbox-gl-native/pull/16136))

  If there are pending requests that are completed after main scheduler is destructed, worker threads may try to push messages to destructed scheduler's queue.

- [core] Fix a crash in GeoJSON source parsing, caused by `GeoJSONVTData` ownership error ([#16106](https://github.com/mapbox/mapbox-gl-native/pull/16106))

- [core] Stable position of labels at tile borders in tile mode ([#16040](https://github.com/mapbox/mapbox-gl-native/pull/16040))

  These changes allow to avoid cutting-off labels on tile borders if the variable text placement is enabled.

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

### 🏁 Performance improvements

- [core] Cross tile index performance ([#16127](https://github.com/mapbox/mapbox-gl-native/pull/16127))

  For overscaled tiles, index only the symbols inside the viewport.

  Find matches only among the buckets that have the same leader Id.

- [core] Calculate GeoJSON tile geometries in a background thread ([#15953](https://github.com/mapbox/mapbox-gl-native/pull/15953))

  Call `mapbox::geojsonvt::GeoJSONVT::getTile()` in a background thread, so that the rendering thread is not blocked.

- [core] Make requests for expired resources lower priority than requests for new resources ([#15950](https://github.com/mapbox/mapbox-gl-native/pull/15950))

- [core] Add Scheduler::scheduleAndReplyValue() API ([#15885](https://github.com/mapbox/mapbox-gl-native/pull/15885))

  This change converts GeoJSON features to tiles for the loaded source description in a background thread and thus unblock the UI thread.

- [core] Enable incremental vacuum for Offline DB ([#15837](https://github.com/mapbox/mapbox-gl-native/pull/15837))

  Thus we avoid re-creating the whole database and keeping the backup file as it happens on calling VACUUM.

- [core] Fix image requests for already obtained images ([#15825](https://github.com/mapbox/mapbox-gl-native/pull/15825))

  Before this fix, repeated request for an already obtained image was erroneously treated as pending, and it prevented from the tiles load completion.

### 🧩  Architectural changes

- [core] Merge style::Layer::set{Layout,Paint}Property ([#15997](https://github.com/mapbox/mapbox-gl-native/pull/15997))

- [core] Use expected.hpp from mapbox-base ([#15898](https://github.com/mapbox/mapbox-gl-native/pull/15898))

##### ⚠️  Breaking changes

- [core] Cross tile index performance (#16127)

  Introduces public [mblg::Renderer::render](https://github.com/mapbox/mapbox-gl-native/pull/16127/files#diff-5a977e0401792825d7fcf522d48df11fR34) API break.

- [core] Refactor DefaultFileSource codebase ([#15768](https://github.com/mapbox/mapbox-gl-native/pull/15768)) 
  - Adds `FileSourceManager` interface that provides access to `FileSource` instances and means of registering / unregistering `FileSource` factories
  - Splits `DefaultFileSource` into smaller parts
  - Adds `DatabaseFileSource` interface and it's default implementation
  - Removes inter-dependencies between concrete `FileSource` classes
  - All sources operate on dedicated thread, except `MainResourceLoader` that acts as a dispatcher and works on thread that requested it.
  - Removes `ResourceOptions::withCacheOnlyRequestsSupport` method

- [core] Remove Map::cycleDebugOptions ([#16005](https://github.com/mapbox/mapbox-gl-native/pull/16005))

  This function was mostly used by the Android API, which is no longer necessary.
