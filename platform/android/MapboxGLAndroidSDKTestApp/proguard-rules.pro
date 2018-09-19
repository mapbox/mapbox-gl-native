# Mapbox ProGuard configuration is handled in the SDK
-dontwarn android.support.**
-dontwarn java.lang.**
-dontwarn org.codehaus.**
-dontwarn java.nio.**

-keep class com.mapbox.mapboxsdk.testapp.model.customlayer.ExampleCustomLayer { *; }
