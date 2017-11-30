#pragma once

#include <mbgl/util/image.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class AndroidLocalGlyphRasterizer {
public:
    static PremultipliedImage drawGlyphBitmap(const FontStack&, unsigned short glyphID, Size size);

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/text/LocalGlyphRasterizer"; };

    static jni::Class<AndroidLocalGlyphRasterizer> javaClass;

    static void registerNative(jni::JNIEnv&);

};

} // namespace android
} // namespace mbgl
