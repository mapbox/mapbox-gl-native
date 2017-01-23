# By default, the flags in this file are appended to flags specified
# in ../sdk/tools/proguard/proguard-android.txt,
# contents of this file will be appended into proguard-android.txt
-keepattributes Signature, *Annotation*, EnclosingMethod

# Square okio, ignoring warnings,
# see https://github.com/square/okio/issues/60
-dontwarn okhttp3.**
-dontwarn okio.**

# Gesture package
-keep class almeros.android.multitouch.gesturedetectors.** { *; }

# Package annotations
-keep class com.mapbox.mapboxsdk.annotations.** { *; }

# Package camera
-keep class com.mapbox.mapboxsdk.camera.** { *; }

# Package: geometry
-keep class com.mapbox.mapboxsdk.geometry.** { *; }

# Package: http
-keep class com.mapbox.mapboxsdk.http.** { *; }

# Package maps
-keep class com.mapbox.mapboxsdk.maps.** { *; }

# Package net
-keep class com.mapbox.mapboxsdk.net.** { *; }

# Package offline
-keep class com.mapbox.mapboxsdk.offline.** { *; }

# Package style
-keep class com.mapbox.mapboxsdk.style.layers.** { *; }
-keep class com.mapbox.mapboxsdk.style.sources.** { *; }

# Package telemetry
-keep class com.mapbox.mapboxsdk.telemetry.** { *; }

#
# Mapbox-java Proguard rules
# We include these rules since libjava is a Jar file not AAR
#

# Retrofit 2
# Platform calls Class.forName on types which do not exist on Android to determine platform.
-dontnote retrofit2.Platform
# Platform used when running on RoboVM on iOS. Will not be used at runtime.
-dontnote retrofit2.Platform$IOS$MainThreadExecutor
# Platform used when running on Java 8 VMs. Will not be used at runtime.
-dontwarn retrofit2.Platform$Java8
# Retain generic type information for use by reflection by converters and adapters.
-keepattributes Signature
# Retain declared checked exceptions for use by a Proxy instance.
-keepattributes Exceptions

# For using GSON @Expose annotation
-keepattributes *Annotation*
# Gson specific classes
-dontwarn sun.misc.**

# Prevent proguard from stripping interface information from TypeAdapterFactory,
# JsonSerializer, JsonDeserializer instances (so they can be used in @JsonAdapter)
-keep class * implements com.google.gson.TypeAdapterFactory
-keep class * implements com.google.gson.JsonSerializer
-keep class * implements com.google.gson.JsonDeserializer

# MAS Data Models
-keep class com.mapbox.services.commons.geojson.** { *; }
-keep class com.mapbox.services.mapmatching.v4.models.** { *; }
-keep class com.mapbox.services.distance.v1.models.** { *; }
-keep class com.mapbox.services.directions.v4.models.** { *; }
-keep class com.mapbox.services.directions.v5.models.** { *; }
-keep class com.mapbox.services.geocoding.v5.models.** { *; }

-dontwarn javax.annotation.**

-keepclassmembers class rx.internal.util.unsafe.** {
    long producerIndex;
    long consumerIndex;
}

-keep class com.google.** { *; }
-dontwarn com.google.**