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
