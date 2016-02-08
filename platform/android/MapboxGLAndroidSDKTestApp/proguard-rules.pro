# Mapbox ProGuard configuration is handled in the SDK

-keep class android.support.** { *; }

-dontwarn retrofit.**
-keep class retrofit.** { *; }
-keepattributes Signature
-keepattributes Exceptions