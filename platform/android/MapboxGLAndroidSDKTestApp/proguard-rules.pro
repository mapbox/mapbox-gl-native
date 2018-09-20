# Mapbox ProGuard configuration is handled in the SDK,
# This file contains test app specific configuration

# Kotlin
-dontnote kotlin.**

# LeakCanary
-dontnote com.squareup.leakcanary.internal.**
-dontnote gnu.trove.THashMap

-keep class com.mapbox.mapboxsdk.testapp.model.customlayer.ExampleCustomLayer { *; }
