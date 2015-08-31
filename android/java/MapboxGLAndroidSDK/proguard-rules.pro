# By default, the flags in this file are appended to flags specified
# in ../sdk/tools/proguard/proguard-android.txt,
# contents of this file will be appended into proguard-android.txt

# Square okio, ignoring warnings,
# see https://github.com/square/okio/issues/60
-dontwarn okio.**

# Package: annotations
-keep class com.mapbox.mapboxgl.annotations.** { *; }

# Package: geometry
-keep class com.mapbox.mapboxgl.geometry.** { *; }

# Package: http
-keep class com.mapbox.mapboxgl.http.** { *; }

# Package views
-keep class com.mapbox.mapboxgl.views.MapView**
-keep class com.mapbox.mapboxgl.views.NativeMapView**
-keepclassmembers class com.mapbox.mapboxgl.views.NativeMapView** {
    *;
}

