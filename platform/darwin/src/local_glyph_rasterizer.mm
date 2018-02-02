#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/platform.hpp>

#include <unordered_map>

#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>
#import <ImageIO/ImageIO.h>

#import "CFHandle.hpp"

namespace mbgl {

/*
    Darwin implementation of LocalGlyphRasterizer:
     Draws CJK glyphs using locally available fonts.
 
    Mirrors GL JS implementation in that:
     - Only CJK glyphs are drawn locally (because we can guess their metrics effectively)
        * Render size/metrics determined experimentally by rendering a few different fonts
     - Configuration is done at map creation time by setting a "font family"
        * JS uses a CSS font-family, this uses kCTFontFamilyNameAttribute which has
          somewhat different behavior.
 
    Further improvements are possible:
     - GL JS heuristically determines a font weight based on the strings included in
        the FontStack. Android follows a simpler heuristic that just picks up the
        "Bold" property from the FontStack. Although both should be possible with CoreText,
        our initial implementation couldn't reliably control the font-weight, so we're
        skipping that functionality on darwin.
        (See commit history for attempted implementation)
     - If we could reliably extract glyph metrics, we wouldn't be limited to CJK glyphs
     - We could push the font configuration down to individual style layers, which would
        allow any current style to be reproducible using local fonts.
     - Instead of just exposing "font family" as a configuration, we could expose a richer
        CTFontDescriptor configuration option (although we'd have to override font size to
        make sure it stayed at 24pt).
     - Because Apple exposes glyph paths via `CTFontCreatePathForGlyph` we could potentially
        render directly to SDF instead of going through TinySDF -- although it's not clear
        how much of an improvement it would be.
*/

using CGColorSpaceHandle = CFHandle<CGColorSpaceRef, CGColorSpaceRef, CGColorSpaceRelease>;
using CGContextHandle = CFHandle<CGContextRef, CGContextRef, CGContextRelease>;
using CFStringRefHandle = CFHandle<CFStringRef, CFTypeRef, CFRelease>;
using CFAttributedStringRefHandle = CFHandle<CFAttributedStringRef, CFTypeRef, CFRelease>;
using CFDictionaryRefHandle = CFHandle<CFDictionaryRef, CFTypeRef, CFRelease>;
using CTFontDescriptorRefHandle = CFHandle<CTFontDescriptorRef, CFTypeRef, CFRelease>;
using CTLineRefHandle = CFHandle<CTLineRef, CFTypeRef, CFRelease>;

class LocalGlyphRasterizer::Impl {
public:
    Impl(const optional<std::string> fontFamily_)
        : fontFamily(fontFamily_)
    {}
    
    ~Impl() {
        for (auto& pair : fontHandles) {
            CFRelease(pair.second);
        }
    }

    
    CTFontRef getFont(const std::u16string& fontName) {
        
        if (fontHandles.find(fontName) == fontHandles.end()) {
            // TODO: Make sure these are the right attributes
          NSDictionary *fontAttributes = @{
                (NSString *)kCTFontSizeAttribute: [NSNumber numberWithFloat:24.0],
                (NSString *)kCTFontFamilyNameAttribute: [NSString stringWithCharacters:(UniChar*)fontName.data() length:fontName.length()]
            };

            CTFontDescriptorRefHandle descriptor(CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes));
            CTFontRef font = CTFontCreateWithFontDescriptor(*descriptor, 0.0, NULL);
            if (!font) {
                throw std::runtime_error("CTFontCreateWithFontDescriptor failed");
            }
            fontHandles[fontName] = font;

        }
        return fontHandles[fontName];
    }
    
private:
    optional<std::string> fontFamily;
    std::unordered_map<std::u16string, CTFontRef> fontHandles;
};

LocalGlyphRasterizer::LocalGlyphRasterizer(const optional<std::string> fontFamily)
    : impl(std::make_unique<Impl>(fontFamily))
{}

LocalGlyphRasterizer::~LocalGlyphRasterizer()
{}

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack&, GlyphID) {
    return true;
    //return util::i18n::allowsFixedWidthGlyphGeneration(glyphID) && impl->getFont();
}

PremultipliedImage drawGlyphBitmap(CGGlyph glyphID, CTFontRef font, GlyphMetrics& metrics) {
    
    CGRect boundingRects[1];
    CGGlyph glyphs[1];
    glyphs[0] = glyphID;
    CTFontGetBoundingRectsForGlyphs(font, kCTFontOrientationHorizontal, glyphs, boundingRects, 1);
    metrics.width = std::ceil(boundingRects[0].size.width) + 6;
    metrics.height = std::ceil(boundingRects[0].size.height) + 16;
    metrics.left = -3;
    metrics.top = -13;
    metrics.advance = 24;
    
    Size size(metrics.width, metrics.height);
    
    PremultipliedImage rgbaBitmap(size);

    CGColorSpaceHandle colorSpace(CGColorSpaceCreateDeviceRGB());
    if (!colorSpace) {
        throw std::runtime_error("CGColorSpaceCreateDeviceRGB failed");
    }
    
    constexpr const size_t bitsPerComponent = 8;
    constexpr const size_t bytesPerPixel = 4;
    const size_t bytesPerRow = bytesPerPixel * size.width;

    CGContextHandle context(CGBitmapContextCreate(
        rgbaBitmap.data.get(),
        size.width,
        size.height,
        bitsPerComponent,
        bytesPerRow,
        *colorSpace,
        kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast));
    if (!context) {
        throw std::runtime_error("CGBitmapContextCreate failed");
    }

    CGPoint positions[1];
    positions[0] = CGPointMake(3.0, 13.0);

    CTFontDrawGlyphs(font, glyphs, positions, 1, *context);
    
    return rgbaBitmap;
}


Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack&, GlyphID glyphID) {
    Glyph manufacturedGlyph;
    CTFontRef font = impl->getFont(glyphID.first);
    if (!font) {
        return manufacturedGlyph;
    }
    
    manufacturedGlyph.id = glyphID;
    
    PremultipliedImage rgbaBitmap = drawGlyphBitmap(glyphID.second, font, manufacturedGlyph.metrics);
   
    Size size(manufacturedGlyph.metrics.width, manufacturedGlyph.metrics.height);
    // Copy alpha values from RGBA bitmap into the AlphaImage output
    manufacturedGlyph.bitmap = AlphaImage(size);
    for (uint32_t i = 0; i < size.width * size.height; i++) {
        manufacturedGlyph.bitmap.data[i] = rgbaBitmap.data[4 * i + 3];
    }

    return manufacturedGlyph;
}

} // namespace mbgl
