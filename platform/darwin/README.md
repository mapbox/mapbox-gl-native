# Darwin

The code in the Darwin platform targets Apple platforms but is not specific
to iOS or macOS. This code is not distributed as an SDK in itself, but is required
by the [Mapbox iOS SDK](https://github.com/mapbox/mapbox-gl-native/tree/master/platform/ios)
and [Mapbox macOS SDK](https://github.com/mapbox/mapbox-gl-native/tree/master/platform/macos).

These files depend on the Foundation and Core Foundation frameworks but do not
depend on iOS- or macOS–specific frameworks, such as UIKit or AppKit. Any
non-cross-platform code is guarded by TargetConditionals.h macros.
