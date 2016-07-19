# 3.3.0

- Adds runtime styling API ([#5318](https://github.com/mapbox/mapbox-gl-native/pull/5318), [#5380](https://github.com/mapbox/mapbox-gl-native/pull/5380), [#5428](https://github.com/mapbox/mapbox-gl-native/pull/5428), [#5429](https://github.com/mapbox/mapbox-gl-native/pull/5429), [#5462](https://github.com/mapbox/mapbox-gl-native/pull/5462), [#5614](https://github.com/mapbox/mapbox-gl-native/pull/5614), [#5670](https://github.com/mapbox/mapbox-gl-native/pull/5670))
- Adds `BUILDTYPE=Debug` support to `make node` ([#5474](https://github.com/mapbox/mapbox-gl-native/pull/5474))
- Fixes a memory leak in `NodeRequest` ([#5529](https://github.com/mapbox/mapbox-gl-native/pull/5529))

# 3.2.1

- Fixes a memory leak in raster image data ([#5269](https://github.com/mapbox/mapbox-gl-native/pull/5269))

# 3.2.0

- Switches to [earcut.hpp](https://github.com/mapbox/earcut.hpp) for tessellation ([#2444](https://github.com/mapbox/mapbox-gl-native/pull/2444))

# 3.1.3

- Fixes a leak in TexturePoolHolder ([#5141](https://github.com/mapbox/mapbox-gl-native/pull/5141))
- Fixes a bug where a callback would be fired after an AsyncRequest had been cancelled ([#5162](https://github.com/mapbox/mapbox-gl-native/pull/5162))

# 3.1.2

- Fixes a race condition with animated transitions ([#4836](https://github.com/mapbox/mapbox-gl-native/pull/4836))

# 3.1.1

- Moves node-pre-gyp from `bundledDependencies` to `preinstall` ([#4680](https://github.com/mapbox/mapbox-gl-native/pull/4680))

# 3.1.0

- Adds debug render options ([#3840](https://github.com/mapbox/mapbox-gl-native/pull/3840))
- Fixes circle bucket rendering on tile boundaries ([#3764](https://github.com/mapbox/mapbox-gl-native/issues/3764))
- Fixes a segfault caused by improperly disposing the entire module in the `NodeLog` destructor ([#4639](https://github.com/mapbox/mapbox-gl-native/pull/4639))
- Fixes an issue with vanishing GeoJSON layers at high zoom levels ([#4632](https://github.com/mapbox/mapbox-gl-native/issues/4632))
- Fixes inheritance from EventEmitter ([#4567](https://github.com/mapbox/mapbox-gl-native/pull/4576))
- Fixes intermittent `stencil mask overflow` error ([#962](https://github.com/mapbox/mapbox-gl-native/issues/962))
- Drops support for Node.js v5.x prebuilt binaries due to ongoing npm3 instability ([#4370](https://github.com/mapbox/mapbox-gl-native/issues/4370))

# 3.0.2

- Fixes a memory leak in `NodeMap::request` ([#3829](https://github.com/mapbox/mapbox-gl-native/pull/3829))
- Increases default max zoom level from 18 to 20 ([#3712](https://github.com/mapbox/mapbox-gl-native/pull/3712))
- Support tiles with non-4096 extents ([#3766](https://github.com/mapbox/mapbox-gl-native/pull/3766))

# 3.0.1

- Fixes missing icon collision boxes ([#3672](https://github.com/mapbox/mapbox-gl-native/pull/3672))
- Fixes texture filtering to draw sharper icons ([#3669](https://github.com/mapbox/mapbox-gl-native/pull/3669))


# 3.0.0

- Drops support for Node.js v0.10.x ([#3635](https://github.com/mapbox/mapbox-gl-native/pull/3635))
- Fixes label clipping issues with `symbol-avoid-edges` ([#3623](https://github.com/mapbox/mapbox-gl-native/pull/3623))
- Avoids label placement around sharp zig-zags ([#3640](https://github.com/mapbox/mapbox-gl-native/pull/3640))

# 2.2.2

- Fixes a bug with non-deterministic label placement [#3543](https://github.com/mapbox/mapbox-gl-native/pull/3543)

# 2.2.1

- Fixes a bug which clipped labels at tile boundaries [#2829](https://github.com/mapbox/mapbox-gl-native/pull/2829)

# 2.2.0

- Adds support for GeoJSON sources [#2161](https://github.com/mapbox/mapbox-gl-native/pull/2161)

# 2.1.0

- Adds [`line-offset`](https://github.com/mapbox/mapbox-gl/issues/3) style property support

# 2.0.1

- Test and publish binaries for Node.js v5.x. ([#3129](https://github.com/mapbox/mapbox-gl-native/pull/3129))

# 2.0.0

- Integrates Node.js bindings into core mapbox-gl-native project. ([#2179](https://github.com/mapbox/mapbox-gl-native/pull/2179))
- Adds Node.js v4.x and io.js v3.x support. ([#2261](https://github.com/mapbox/mapbox-gl-native/pull/2261))
- Requires an options object argument to `new mbgl.Map()`
  (with required `request` and optional `cancel` methods),
  drops `mbgl.FileSource`. ([mapbox/node-mapbox-gl-native#143](https://github.com/mapbox/node-mapbox-gl-native/pull/143))
- Changes `request` semantics to pass a second, callback argument instead
  of needing to call `req.respond`. ([#2299](https://github.com/mapbox/mapbox-gl-native/pull/2299))
- Accepts optional `ratio` (defaults to `1.0`) in `mbgl.Map` options
  argument. Map pixel ratio is now immutable and can no longer be set with
  render options. ([`a8d9b92`](https://github.com/mapbox/mapbox-gl-native/commit/a8d9b921d71a91d7f8eff82e5a584aaab8b7d1c6), [#1799](https://github.com/mapbox/mapbox-gl-native/pull/1799), [#2937](https://github.com/mapbox/mapbox-gl-native/pull/2937))
- Swaps array order in render options `center` argument to `[lng, lat]` for consistency with GeoJSON and mapbox-gl-js. ([#2935](https://github.com/mapbox/mapbox-gl-native/pull/2935))
- Adds render option `pitch`. ([#2702](https://github.com/mapbox/mapbox-gl-native/pull/2702))
- `map.render` now returns a raw image buffer instead of an object with
  `width`, `height` and `pixels` properties. ([#2262](https://github.com/mapbox/mapbox-gl-native/pull/2262))
- Adds support for rendering [mapbox-gl-style-spec](https://github.com/mapbox/mapbox-gl-style-spec) v8 styles.
- No longer loads resources before a render request is made. ([`55d25a8`](https://github.com/mapbox/mapbox-gl-native/commit/55d25a80a77c06ef5e66acc0d8518867b03fe8a4))
- Fixes a bug which prevented raster tiles that `404`'ed from rendering. ([#2458](https://github.com/mapbox/mapbox-gl-native/pull/2458))
- Fade transitions are now ignored to prevent half faded labels. ([#942](https://github.com/mapbox/mapbox-gl-native/pull/942))
- Labels can now line wrap on hyphens and other punctuation. ([#2598](https://github.com/mapbox/mapbox-gl-native/pull/2598))

# 1.1.3

- Removes deprecated mbgl::Environment from NodeLogObserver.

# 1.1.2

- Check libuv version semver-ishly, fixes segfaults in Node.js 0.12.x
  and io.js.
- Fixes segfault, throws JavaScript error instead when attempting to 
  render without first loading a style.
- Bumps mbgl submodule to v0.4.0

# 1.1.1

- Bumps mbgl submodule to v0.3.5

# 1.1.0

- Adds Node.js v0.12.x and io.js support.
- Adds `map.release()` method for manual cleanup of map resources.
- Fixes garbage collection of NodeMap objects.
- Returns an error callback for failed NodeFileSource requests.
- Fixes handling of corrupt NodeFileSource request data.
- Implements request coalescing to fix NodeRequest cancellation.
- Removes `setAccessToken` method from NodeMap, `mapbox://` URLs
  should be handled through `NodeFileSource` now.
- Updates build scripts and Travis CI configuration.
- Logs Environment ID and thread name when in an Environment scope.
- Refactors NodeLog to implement mbgl::Log::Observer.
- Fixes uncaught exception from missing sprites.
- Fixes Unicode glyph range end.

# 1.0.3

- Fixes crash during garbage collection by assigning FileSource handle 
  to a v8::Persistent in NodeMap constructor.

# 1.0.2

- Initialize shared display connection at module load time to avoid 
  race condition when display connection is initialized on-demand.

# 1.0.1

- Adapts NodeFileSource around mbgl::Environment additions.
- Adapts to minor changes in mapbox-gl-test-suite.
- Adds tests for gzipped vector tile handling.
- Cleans up documentation.

# 1.0.0

- Initial release.
