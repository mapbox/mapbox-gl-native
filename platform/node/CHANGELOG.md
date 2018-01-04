# master
- The `Map` constructor now accepts a `mode` option which can be either `"static"` (default) or `"tile"`. It must be set to `"tile"` when rendering individual tiles in order for the symbols to match across tiles.

# 3.5.8 - October 19, 2017
- Fixes an issue that causes memory leaks when not deleting the frontend object
  in NodeMap::release()
- Fixes a crash in Earcut: [#10245](https://github.com/mapbox/mapbox-gl-native/pull/10245)

# 3.5.7 - October 9, 2017
- Fixed an issue causing synchronous resource requests to stall [#10153](https://github.com/mapbox/mapbox-gl-native/pull/10153)

# 3.5.6 - September 29, 2017
- Protects against requests which throw [#9554](https://github.com/mapbox/mapbox-gl-native/pull/9554)
- Fixed an issue around reusing a map object [#9554](https://github.com/mapbox/mapbox-gl-native/pull/9554)
- Fixed an issue in test [#9553](https://github.com/mapbox/mapbox-gl-native/pull/9553)
- Increased the default maximum zoom level from 20 to 22 ([#9835](https://github.com/mapbox/mapbox-gl-native/pull/9835))

# 3.5.5 - July 14, 2017
- Provide debuggable release builds for node packages [#9497](https://github.com/mapbox/mapbox-gl-native/pull/9497)

# 3.5.4 - June 6, 2017
- Add support for ImageSource [#8968](https://github.com/mapbox/mapbox-gl-native/pull/8968)
- Fixed an issue with `map.addImage()` which would cause added images to randomly be replaced with images found the style's sprite sheet ([#9119](https://github.com/mapbox/mapbox-gl-native/pull/9119))

# 3.5.3 - May 30, 2017

- Fixed a regression around `line-dasharrary` and `fill-pattern` that caused these properties to sometimes not render correctly ([#9130](https://github.com/mapbox/mapbox-gl-native/pull/9130))

# 3.5.2 - May 18, 2017

- Fixed a memory leak ([#8884](https://github.com/mapbox/mapbox-gl-native/pull/9035))


# 3.5.1 - May 8, 2017

- Adds Node v6 binaries. **Note, Node v4 binaries will be removed on August 1st.** ([#8884](https://github.com/mapbox/mapbox-gl-native/pull/8884))
- Adds linux debug binaries ([#8865](https://github.com/mapbox/mapbox-gl-native/pull/8865))

# 3.5.0 - April 20, 2017

- Fixed an issue where raster tiles that were not found caused `map.render()` to hang ([#8769](https://github.com/mapbox/mapbox-gl-native/pull/8769))
- Adds method `map.cancel()` which cancels an ongoing `render` call. ([#8249](https://github.com/mapbox/mapbox-gl-native/pull/8249))

# 3.4.7 - March 15, 2017

- Fixed MacOS Release builds ([8409](https://github.com/mapbox/mapbox-gl-native/pull/8409))

# 3.4.6 - March 14, 2017

- Publishes `Release` build on Mac ([#8407](https://github.com/mapbox/mapbox-gl-native/pull/8407))
- Fixes the publish binary build process ([#8406](https://github.com/mapbox/mapbox-gl-native/pull/8406))

# 3.4.5 - March 14, 2017

- Fixed a memory hang issue after GlyphAtlas was refactored ([#8394](https://github.com/mapbox/mapbox-gl-native/pull/8394))

# 3.4.4 - January 10, 2017

- Updates the node binary publish location on s3 to reflect new package name ([#7653](https://github.com/mapbox/mapbox-gl-native/pull/7653))

# 3.4.3 - January 9, 2017

- Adds `map.addImage()` and `map.removeImage()` APIs ([#7610](https://github.com/mapbox/mapbox-gl-native/pull/7610))

# 3.4.2 - November 15, 2016

- Switches back to publishing Linux binaries with GLX, to eliminate a runtime dependency on `libOSMesa.so.8` and enable dynamically linking against `libGL.so` provided by an alternate implementation, such as the NVIDIA proproetary drivers ([#7503](https://github.com/mapbox/mapbox-gl-native/pull/7053))

# 3.4.1 - November 10, 2016

- Skips assigning clip IDs to tiles that won't be rendered, mitigating a `stencil mask overflow` error ([#6871](https://github.com/mapbox/mapbox-gl-native/pull/6871))
- Fixes camera logic to avoid unnecessary or redundant setting of camera options ([#6990](https://github.com/mapbox/mapbox-gl-native/pull/6990))

# 3.4.0 - November 2, 2016

- Fixes Bitrise configuration to automatically publish macOS binaries ([#6789](https://github.com/mapbox/mapbox-gl-native/pull/6789))
- Switches from using individual thread pools for each `mbgl::Map` object to sharing the built-in Node.js thread pool for NodeMap implementations ([#6687](https://github.com/mapbox/mapbox-gl-native/pull/6687))

# 3.3.3 - September 6, 2016

- Switches to using a NodeRequest member function (with a JavaScript shim in front to preserve the API) instead of a new `v8::Context` to avoid a memory leak ([#5704](https://github.com/mapbox/mapbox-gl-native/pull/5704))
- `map.load` can now throw when failing to parse an invalid style ([#6151](https://github.com/mapbox/mapbox-gl-native/pull/6151))
- Explicitly links the OpenGL framework for compatibility with macOS Sierra ([#6015](https://github.com/mapbox/mapbox-gl-native/pull/6015))

# 3.3.2 - August 1, 2016

- Fixes Node.js binary publishing to build with `BUILDTYPE=Release` ([#5838](https://github.com/mapbox/mapbox-gl-native/pull/5838))

# 3.3.1 - July 29, 2016

- Fixes `minzoom` and `maxzoom` properties ([#5828](https://github.com/mapbox/mapbox-gl-native/pull/5828))
- Fixes `RunLoop::runOnce()` to use `UV_RUN_NOWAIT` instead of `UV_RUN_ONCE` (which can block the libuv threadpool) ([#5758](https://github.com/mapbox/mapbox-gl-native/pull/5758))
- Map debug options 'overdraw' and 'stencil clip' are now disabled (no-ops) in release mode ([#5555](https://github.com/mapbox/mapbox-gl-native/pull/5555))

# 3.3.0 - July 14, 2016

- Adds runtime styling API ([#5318](https://github.com/mapbox/mapbox-gl-native/pull/5318), [#5380](https://github.com/mapbox/mapbox-gl-native/pull/5380), [#5428](https://github.com/mapbox/mapbox-gl-native/pull/5428), [#5429](https://github.com/mapbox/mapbox-gl-native/pull/5429), [#5462](https://github.com/mapbox/mapbox-gl-native/pull/5462), [#5614](https://github.com/mapbox/mapbox-gl-native/pull/5614), [#5670](https://github.com/mapbox/mapbox-gl-native/pull/5670))
- Adds `BUILDTYPE=Debug` support to `make node` ([#5474](https://github.com/mapbox/mapbox-gl-native/pull/5474))
- Fixes a memory leak in `NodeRequest` ([#5529](https://github.com/mapbox/mapbox-gl-native/pull/5529))

# 3.2.1 - June 7, 2016

- Fixes a memory leak in raster image data ([#5269](https://github.com/mapbox/mapbox-gl-native/pull/5269))

# 3.2.0 - June 3, 2016

- Switches to [earcut.hpp](https://github.com/mapbox/earcut.hpp) for tessellation ([#2444](https://github.com/mapbox/mapbox-gl-native/pull/2444))

# 3.1.3 - May 27, 2016

- Fixes a leak in TexturePoolHolder ([#5141](https://github.com/mapbox/mapbox-gl-native/pull/5141))
- Fixes a bug where a callback would be fired after an AsyncRequest had been cancelled ([#5162](https://github.com/mapbox/mapbox-gl-native/pull/5162))

# 3.1.2 - April 26, 2016

- Fixes a race condition with animated transitions ([#4836](https://github.com/mapbox/mapbox-gl-native/pull/4836))

# 3.1.1 - April 11, 2016

- Moves node-pre-gyp from `bundledDependencies` to `preinstall` ([#4680](https://github.com/mapbox/mapbox-gl-native/pull/4680))

# 3.1.0 - April 8, 2016

- Adds debug render options ([#3840](https://github.com/mapbox/mapbox-gl-native/pull/3840))
- Fixes circle bucket rendering on tile boundaries ([#3764](https://github.com/mapbox/mapbox-gl-native/issues/3764))
- Fixes a segfault caused by improperly disposing the entire module in the `NodeLog` destructor ([#4639](https://github.com/mapbox/mapbox-gl-native/pull/4639))
- Fixes an issue with vanishing GeoJSON layers at high zoom levels ([#4632](https://github.com/mapbox/mapbox-gl-native/issues/4632))
- Fixes inheritance from EventEmitter ([#4567](https://github.com/mapbox/mapbox-gl-native/pull/4576))
- Fixes intermittent `stencil mask overflow` error ([#962](https://github.com/mapbox/mapbox-gl-native/issues/962))
- Drops support for Node.js v5.x prebuilt binaries due to ongoing npm3 instability ([#4370](https://github.com/mapbox/mapbox-gl-native/issues/4370))

# 3.0.2 - February 4, 2016

- Fixes a memory leak in `NodeMap::request` ([#3829](https://github.com/mapbox/mapbox-gl-native/pull/3829))
- Increases default max zoom level from 18 to 20 ([#3712](https://github.com/mapbox/mapbox-gl-native/pull/3712))
- Support tiles with non-4096 extents ([#3766](https://github.com/mapbox/mapbox-gl-native/pull/3766))

# 3.0.1 - January 26, 2016

- Fixes missing icon collision boxes ([#3672](https://github.com/mapbox/mapbox-gl-native/pull/3672))
- Fixes texture filtering to draw sharper icons ([#3669](https://github.com/mapbox/mapbox-gl-native/pull/3669))

# 3.0.0 - January 21, 2016

- Drops support for Node.js v0.10.x ([#3635](https://github.com/mapbox/mapbox-gl-native/pull/3635))
- Fixes label clipping issues with `symbol-avoid-edges` ([#3623](https://github.com/mapbox/mapbox-gl-native/pull/3623))
- Avoids label placement around sharp zig-zags ([#3640](https://github.com/mapbox/mapbox-gl-native/pull/3640))

# 2.2.2 - January 19, 2016

- Fixes a bug with non-deterministic label placement [#3543](https://github.com/mapbox/mapbox-gl-native/pull/3543)

# 2.2.1 - January 7, 2016

- Fixes a bug which clipped labels at tile boundaries [#2829](https://github.com/mapbox/mapbox-gl-native/pull/2829)

# 2.2.0 - December 16, 2015

- Adds support for GeoJSON sources [#2161](https://github.com/mapbox/mapbox-gl-native/pull/2161)

# 2.1.0 - December 8, 2015

- Adds [`line-offset`](https://github.com/mapbox/mapbox-gl/issues/3) style property support

# 2.0.1 - November 25, 2015

- Test and publish binaries for Node.js v5.x. ([#3129](https://github.com/mapbox/mapbox-gl-native/pull/3129))

# 2.0.0 - November 24, 2015

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

# 1.1.3 - June 25, 2015

- Removes deprecated mbgl::Environment from NodeLogObserver.

# 1.1.2 - June 22, 2015

- Check libuv version semver-ishly, fixes segfaults in Node.js 0.12.x
  and io.js.
- Fixes segfault, throws JavaScript error instead when attempting to
  render without first loading a style.
- Bumps mbgl submodule to v0.4.0

# 1.1.1 - June 16, 2015

- Bumps mbgl submodule to v0.3.5

# 1.1.0 - June 15, 2015

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

# 1.0.3 - April 3, 2015

- Fixes crash during garbage collection by assigning FileSource handle
  to a v8::Persistent in NodeMap constructor.

# 1.0.2 - April 2, 2015

- Initialize shared display connection at module load time to avoid
  race condition when display connection is initialized on-demand.

# 1.0.1 - March 19, 2015

- Adapts NodeFileSource around mbgl::Environment additions.
- Adapts to minor changes in mapbox-gl-test-suite.
- Adds tests for gzipped vector tile handling.
- Cleans up documentation.

# 1.0.0 - February 25, 2015

- Initial release.
