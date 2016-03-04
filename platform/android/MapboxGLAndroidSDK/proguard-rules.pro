# By default, the flags in this file are appended to flags specified
# in ../sdk/tools/proguard/proguard-android.txt,
# contents of this file will be appended into proguard-android.txt

# Square okio, ignoring warnings,
# see https://github.com/square/okio/issues/60
-dontwarn okio.**

# Package: annotations
-keep class com.mapbox.mapboxsdk.annotations.** { *; }

# Package: geometry
-keep class com.mapbox.mapboxsdk.geometry.** { *; }

# Package: http
-keep class com.mapbox.mapboxsdk.http.** { *; }

# Package maps
-keep class com.mapbox.mapboxsdk.maps.** { *; }

# Package telemetry
-keep class com.mapbox.mapboxsdk.telemetry.** { *; }

# Package layers
-keep class com.mapbox.mapboxsdk.layers.** { *; }

# Package camera
-keep class com.mapbox.mapboxsdk.camera.** { *; }

# Package offline
-keep class com.mapbox.mapboxsdk.offline.** { *; }

# Gesture package
-keep class almeros.android.multitouch.gesturedetectors.** { *; }
