# Changelog

## Release 20191115

### Bug fixes
- [core] Avoid edges for labels that use text-variable-anchors ([#15854](https://github.com/mapbox/mapbox-gl-native/pull/15854))

    Fix prevents label clipping when Tiled rendering mode is used.

- [core] Fix icon-text-fit ([#15634](https://github.com/mapbox/mapbox-gl-native/pull/15634))

    This fixes rendering by account for the 1px texture padding around icons that were stretched with icon-text-fit.

- [core] Fix collisions with icon-text-fit and variable placement ([#15828](https://github.com/mapbox/mapbox-gl-native/pull/15828))

  When deciding the placement position it was only checking whether there were text collisions. Now, if icon-text-fit is used it now checks whether the icon fits before accepting a placement position.

- [core] Fix opacity interpolation for composition expressions ([#15738](https://github.com/mapbox/mapbox-gl-native/pull/15738))

    Previously the interpolation factor could be out of the range 0 to 1 which could result in rendering errors.

- [core] Fix performance-move-const-arg ([#15710](https://github.com/mapbox/mapbox-gl-native/pull/15710))
- [core] Fix bugprone-use-after-move ([#15710](https://github.com/mapbox/mapbox-gl-native/pull/15710))

### Performance improvements
- [core] Suppress network requests for invisible tiles ([#15741](https://github.com/mapbox/mapbox-gl-native/pull/15741))

  If the render source does not need rendering, we set necessity for its tiles to optional, and thus suppress network requests on tiles expiration.

- [core] Enable incremental vacuum for Offline DB ([#15837](https://github.com/mapbox/mapbox-gl-native/pull/15837))

    Thus we avoid re-creating the whole database and keeping the backup file as it happens on calling VACUUM.

- [core] No pending requests for already requested images ([#15864](https://github.com/mapbox/mapbox-gl-native/pull/15864))

    Instead of coalescing requests we just do not treat requests for already requested images as pending (i.e. waiting to be dispatched to the client).

- [core] Orchestration performance for styles with multiple sources ([#15756](https://github.com/mapbox/mapbox-gl-native/pull/15756))

    RenderOrchestrator holds a vector of ordered rendered layers, in order to minimize calling of getRenderLayer(), which could be quite expensive as it calculates hash of the layer Id.

### New features

- None

### Architectural / design changes

- [core] Add universal style getter API ([#15708](https://github.com/mapbox/mapbox-gl-native/pull/15708), [#15720](https://github.com/mapbox/mapbox-gl-native/pull/15720))

- [core] Remove unaccent from platform interfaces ([#15702](https://github.com/mapbox/mapbox-gl-native/pull/15702))

- [build] Move compression to platform ([#15692](https://github.com/mapbox/mapbox-gl-native/pull/15692))