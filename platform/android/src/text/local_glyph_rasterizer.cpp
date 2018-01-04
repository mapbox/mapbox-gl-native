#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/platform.hpp>

#include <jni/jni.hpp>

#include "../attach_env.hpp"
#include "../bitmap.hpp"

#include "local_glyph_rasterizer_jni.hpp"

/*
    Android implementation of LocalGlyphRasterizer:
     Draws CJK glyphs using locally available fonts.

    Follows pattern of GL JS implementation in that:
     - Only CJK glyphs are drawn locally (because we can guess their metrics effectively)
        * Render size/metrics determined experimentally using Noto Sans
     - Configuration is done at map creation time by setting a "font family"
        * JS uses a CSS font-family, this uses android.graphics.Typeface
          https://developer.android.com/reference/android/graphics/Typeface.html
     - We use heuristics to extract a font-weight based on the incoming font stack
        * JS tries to extract multiple weights, this implementation only looks for
          "bold"

     mbgl::LocalGlyphRasterizer is the portable interface
     mbgl::LocalGlyphRasterizer::Impl stores platform-specific configuration data
     mbgl::android::LocalGlyphRasterizer is the JNI wrapper
     com.mapbox.mapboxsdk.text.LocalGlyphRasterizer is the Java implementation that
      actually does the drawing
 */

namespace mbgl {
namespace android {

PremultipliedImage LocalGlyphRasterizer::drawGlyphBitmap(const std::string& fontFamily, const bool bold, const GlyphID glyphID) {
    UniqueEnv env { AttachEnv() };

    using Signature = jni::Object<Bitmap>(jni::String, jni::jboolean, jni::jchar);
    auto method = javaClass.GetStaticMethod<Signature>(*env, "drawGlyphBitmap");

    jni::String jniFontFamily = jni::Make<jni::String>(*env, fontFamily);

    auto javaBitmap = javaClass.Call(*env,
                                     method,
                                     jniFontFamily,
                                     static_cast<jni::jboolean>(bold),
                                     static_cast<jni::jchar>(glyphID));

    PremultipliedImage result = Bitmap::GetImage(*env, javaBitmap);
    jni::DeleteLocalRef(*env, javaBitmap);
    return result;
}

void LocalGlyphRasterizer::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<LocalGlyphRasterizer>::Find(env).NewGlobalRef(env).release();
}

jni::Class<LocalGlyphRasterizer> LocalGlyphRasterizer::javaClass;

} // namespace android

class LocalGlyphRasterizer::Impl {
public:
    Impl(const optional<std::string> fontFamily_)
        : fontFamily(fontFamily_)
    {}

    bool isConfigured() const {
        return bool(fontFamily);
    }

    PremultipliedImage drawGlyphBitmap(const FontStack& fontStack, GlyphID glyphID) {
        bool bold = false;
        for (auto font : fontStack) {
            std::string lowercaseFont = platform::lowercase(font);
            if (lowercaseFont.find("bold") != std::string::npos) {
                bold = true;
            }
        }
        return android::LocalGlyphRasterizer::drawGlyphBitmap(*fontFamily, bold, glyphID);
    }

private:
    optional<std::string> fontFamily;
};

LocalGlyphRasterizer::LocalGlyphRasterizer(const optional<std::string> fontFamily)
    : impl(std::make_unique<Impl>(fontFamily))
{}

LocalGlyphRasterizer::~LocalGlyphRasterizer()
{}

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack&, GlyphID glyphID) {
     return util::i18n::allowsFixedWidthGlyphGeneration(glyphID) && impl->isConfigured();
}

Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack& fontStack, GlyphID glyphID) {
    Glyph fixedMetrics;
    if (!impl->isConfigured()) {
        return fixedMetrics;
    }

    fixedMetrics.id = glyphID;

    Size size(35, 35);

    fixedMetrics.metrics.width = size.width;
    fixedMetrics.metrics.height = size.height;
    fixedMetrics.metrics.left = 3;
    fixedMetrics.metrics.top = -10;
    fixedMetrics.metrics.advance = 24;

    PremultipliedImage rgbaBitmap = impl->drawGlyphBitmap(fontStack, glyphID);

    // Copy alpha values from RGBA bitmap into the AlphaImage output
    fixedMetrics.bitmap = AlphaImage(size);
    for (uint32_t i = 0; i < size.width * size.height; i++) {
        fixedMetrics.bitmap.data[i] = rgbaBitmap.data[4 * i + 3];
    }

    return fixedMetrics;
}

} // namespace mbgl
