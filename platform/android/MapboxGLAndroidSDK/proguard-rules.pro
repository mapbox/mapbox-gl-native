# By default, the flags in this file are appended to flags specified
# in ../sdk/tools/proguard/proguard-android.txt,
# contents of this file will be appended into proguard-android.txt
-keepattributes Signature, *Annotation*

# Square okio, ignoring warnings,
# see https://github.com/square/okio/issues/60
-dontwarn okio.**

# Gesture package
-keep class almeros.android.multitouch.gesturedetectors.** { *; }

# Package: annotations
-keep class com.mapbox.mapboxsdk.annotations.** { *; }

# Package camera
-keep class com.mapbox.mapboxsdk.camera.** { *; }

# Package: geometry
-keep class com.mapbox.mapboxsdk.geometry.** { *; }

# Package: http
-keep class com.mapbox.mapboxsdk.http.** { *; }

# Package maps
-keep class com.mapbox.mapboxsdk.maps.** { *; }

# Package offline
-keep class com.mapbox.mapboxsdk.offline.** { *; }

# Package style
-keep class com.mapbox.mapboxsdk.style.layers.** { *; }
-keep class com.mapbox.mapboxsdk.style.sources.** { *; }

# Package telemetry
-keep class com.mapbox.mapboxsdk.telemetry.** { *; }

# Keep external project mapbox-java,
# Needs to be removed after https://github.com/mapbox/mapbox-java/issues/178 is resolved
-keep class com.mapbox.services.** { *; }

