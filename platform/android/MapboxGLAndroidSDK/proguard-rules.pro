# By default, the flags in this file are appended to flags specified
# in ../sdk/tools/proguard/proguard-android.txt,
# contents of this file will be appended into proguard-android.txt
-keepattributes Signature, *Annotation*, EnclosingMethod

# config for the SDK
-keep class com.mapbox.mapboxsdk.** { *; }
-keep interface com.mapbox.mapboxsdk.** { *; }

# config for okhttp 3.8.0, https://github.com/square/okhttp/pull/3354
-dontwarn okio.**
-dontwarn javax.annotation.**
-dontnote okhttp3.**

# config for mapbox-android-telemetry:3.0.0-beta.1
-keep class com.mapbox.android.telemetry.** { *; }
-dontwarn com.mapbox.android.core.location.MockLocationEngine
-dontwarn com.mapbox.android.core.location.MockLocationEngine$LocationUpdateRunnable
-dontwarn java.awt.Color
-dontwarn com.mapzen.android.lost.api**
-dontwarn org.conscrypt.OpenSSLProvider
-dontwarn org.conscrypt.Conscrypt

# config for mapbox-sdk-geojson:3.0.1
-keep class com.mapbox.geojson.** { *; }
-keep class com.google.gson.** { *; }
-dontnote com.google.gson.internal.UnsafeAllocator

# config for mapbox-android-gestures:0.0.1-20180228.152340-13
-dontnote com.mapbox.android.gestures.*

# config for additional warnings
-keep class com.google.android.gms.dynamite.descriptors.com.google.android.gms.flags.ModuleDescriptor { java.lang.String MODULE_ID; }
-keep class com.google.android.gms.dynamite.descriptors.com.google.android.gms.flags.ModuleDescriptor { int MODULE_VERSION; }
-keep class com.google.android.gms.dynamite.DynamiteModule$DynamiteLoaderClassLoader { java.lang.ClassLoader sClassLoader; }
-dontnote com.google.devtools.build.android.desugar.runtime.ThrowableExtension
-dontnote org.robolectric.Robolectric
-dontnote libcore.io.Memory
-dontnote com.google.protobuf.ExtensionRegistry
-dontnote com.google.protobuf.Extension
-dontnote android.net.**
-dontnote org.apache.http.**
-dontwarn com.sun.xml.internal.ws.spi.db.**