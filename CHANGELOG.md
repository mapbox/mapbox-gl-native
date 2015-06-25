# 1.1.3

- Removes deprecated mbgl::Environment from NodeLogObserver.

# 1.1.2

- Check libuv version semver-ishly, fixes segfaults in Node.js 0.12.x
  and iojs.
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
