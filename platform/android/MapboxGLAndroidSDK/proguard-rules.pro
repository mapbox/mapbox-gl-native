# By default, the flags in this file are appended to flags specified
# in ../sdk/tools/proguard/proguard-android.txt,
# contents of this file will be appended into proguard-android.txt
-keepattributes Signature, *Annotation*, EnclosingMethod
-keep class almeros.android.multitouch.gesturedetectors.** { *; }
-keep class com.mapbox.mapboxsdk.** { *; }
-keep interface com.mapbox.mapboxsdk.** { *; }
-keep class com.mapbox.services.android.telemetry.** { *; }
-keep class com.mapbox.services.commons.** { *;}