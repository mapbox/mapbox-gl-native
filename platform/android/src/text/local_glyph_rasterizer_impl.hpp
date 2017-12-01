#pragma once

#include <mbgl/util/image.hpp>

#include <jni/jni.hpp>

/*
    android::LocalGlyphRasterizer is the JNI wrapper of
    com/mapbox/mapboxsdk/text/LocalGlyphRasterizer

    mbgl::LocalGlyphRasterizer is the portable interface
    Both implementations are in local_glyph_rasterizer.cpp
 */

namespace mbgl {
namespace android {

class LocalGlyphRasterizer {
public:
    static PremultipliedImage drawGlyphBitmap(const std::string& fontFamily, const bool bold, const char16_t glyphID);

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/text/LocalGlyphRasterizer"; };

    static jni::Class<LocalGlyphRasterizer> javaClass;

    static void registerNative(jni::JNIEnv&);

};

} // namespace android
} // namespace mbgl
