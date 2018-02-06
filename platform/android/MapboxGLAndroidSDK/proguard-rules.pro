# By default, the flags in this file are appended to flags specified
# in ../sdk/tools/proguard/proguard-android.txt,
# contents of this file will be appended into proguard-android.txt
-keepattributes Signature, *Annotation*, EnclosingMethod
-keep class almeros.android.multitouch.gesturedetectors.** { *; }
-keep class com.mapbox.mapboxsdk.** { *; }
-keep interface com.mapbox.mapboxsdk.** { *; }
-keep class com.mapbox.services.android.telemetry.** { *; }
-keep class com.mapbox.services.commons.** { *;}
-keep class com.google.gson.** { *; }

# config for okhttp 3.8.0, https://github.com/square/okhttp/pull/3354
-dontwarn okio.**
-dontwarn javax.annotation.Nullable
-dontwarn javax.annotation.ParametersAreNonnullByDefault

# config for optional location provider https://github.com/mapbox/mapbox-gl-native/issues/10960
-dontwarn com.mapzen.android.lost.api**