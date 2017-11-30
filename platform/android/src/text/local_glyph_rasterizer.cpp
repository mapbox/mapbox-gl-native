#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>

#include <jni/jni.hpp>

#include "../attach_env.hpp"
#include "../bitmap.hpp"

#include "local_glyph_rasterizer_impl.hpp" // Actually AndroidLocalGlyphRasterizer

namespace mbgl {

namespace android {

PremultipliedImage AndroidLocalGlyphRasterizer::drawGlyphBitmap(const FontStack&, unsigned short glyphID, Size) {
    UniqueEnv env { AttachEnv() }; // TODO: How should this be hooked up?

    // TODO: Pass in font stack and size (and configuration)
    // For now, just try to hard-wire any rendering at all
    using Signature = jni::Object<Bitmap>(jni::jchar);
    auto method = javaClass.GetStaticMethod<Signature>(*env, "drawGlyphBitmap");
    auto result = javaClass.Call(*env, method, glyphID);

    return Bitmap::GetImage(*env, result);
}

void AndroidLocalGlyphRasterizer::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<AndroidLocalGlyphRasterizer>::Find(env).NewGlobalRef(env).release();
}

jni::Class<AndroidLocalGlyphRasterizer> AndroidLocalGlyphRasterizer::javaClass;

} // namespace android

class LocalGlyphRasterizer::Impl {
public:
    bool hasFont(const FontStack&) const {
        return true;
    }

    PremultipliedImage drawGlyphBitmap(const FontStack& fontStack, GlyphID glyphID, Size size) {
        return android::AndroidLocalGlyphRasterizer::drawGlyphBitmap(fontStack, (unsigned short)glyphID, size);
    }
};

LocalGlyphRasterizer::LocalGlyphRasterizer(const optional<std::string>)
    : impl(std::make_unique<Impl>())
{}

LocalGlyphRasterizer::~LocalGlyphRasterizer()
{}

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack& fontStack, GlyphID glyphID) {
     return util::i18n::allowsFixedWidthGlyphGeneration(glyphID) && impl->hasFont(fontStack);
}

Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack& fontStack, GlyphID glyphID) {
    Glyph fixedMetrics;
    if (!impl->hasFont(fontStack)) {
        return fixedMetrics;
    }

    fixedMetrics.id = glyphID;

    Size size(35, 35);

    fixedMetrics.metrics.width = size.width;
    fixedMetrics.metrics.height = size.height;
    fixedMetrics.metrics.left = 3;
    fixedMetrics.metrics.top = -10;
    fixedMetrics.metrics.advance = 24;

    PremultipliedImage rgbaBitmap = impl->drawGlyphBitmap(fontStack, glyphID, size);

    // Copy alpha values from RGBA bitmap into the AlphaImage output
    fixedMetrics.bitmap = AlphaImage(size);
    for (uint32_t i = 0; i < size.width * size.height; i++) {
        fixedMetrics.bitmap.data[i] = rgbaBitmap.data[4 * i + 3];
    }

    return fixedMetrics;
}

} // namespace mbgl
