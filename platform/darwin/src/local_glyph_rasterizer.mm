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
        , fontHandle(NULL)
    {}
    
    ~Impl() {
        if (fontHandle) {
            CFRelease(fontHandle);
        }
    }

    
    CTFontRef getFont() {
        if (!fontFamily) {
            return NULL;
        }
        
        if (!fontHandle) {
          NSDictionary *fontAttributes = @{
                (NSString *)kCTFontSizeAttribute: [NSNumber numberWithFloat:24.0],
                (NSString *)kCTFontFamilyNameAttribute: [[NSString alloc] initWithCString:fontFamily->c_str() encoding:NSUTF8StringEncoding]
            };

            CTFontDescriptorRefHandle descriptor(CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes));
            fontHandle = CTFontCreateWithFontDescriptor(*descriptor, 0.0, NULL);
            if (!fontHandle) {
                throw std::runtime_error("CTFontCreateWithFontDescriptor failed");
            }
        }
        return fontHandle;
    }
    
private:
    optional<std::string> fontFamily;
    CTFontRef fontHandle;
};

LocalGlyphRasterizer::LocalGlyphRasterizer(const optional<std::string> fontFamily)
    : impl(std::make_unique<Impl>(fontFamily))
{}

LocalGlyphRasterizer::~LocalGlyphRasterizer()
{}

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack&, GlyphID glyphID) {
    return util::i18n::allowsFixedWidthGlyphGeneration(glyphID) && impl->getFont();
}

PremultipliedImage drawGlyphBitmap(GlyphID glyphID, CTFontRef font, Size size) {
    PremultipliedImage rgbaBitmap(size);
    
    CFStringRefHandle string(CFStringCreateWithCharacters(NULL, reinterpret_cast<UniChar*>(&glyphID), 1));

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

    CFStringRef keys[] = { kCTFontAttributeName };
    CFTypeRef values[] = { font };

    CFDictionaryRefHandle attributes(
        CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
            (const void**)&values, sizeof(keys) / sizeof(keys[0]),
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks));

    CFAttributedStringRefHandle attrString(CFAttributedStringCreate(kCFAllocatorDefault, *string, *attributes));
    CTLineRefHandle line(CTLineCreateWithAttributedString(*attrString));
    
    // Start drawing a little bit below the top of the bitmap
    CGContextSetTextPosition(*context, 0.0, 5.0);
    CTLineDraw(*line, *context);
    
    return rgbaBitmap;
}

Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack&, GlyphID glyphID) {
    Glyph fixedMetrics;
    CTFontRef font = impl->getFont();
    if (!font) {
        return fixedMetrics;
    }
    
    fixedMetrics.id = glyphID;

    Size size(35, 35);
    
    fixedMetrics.metrics.width = size.width;
    fixedMetrics.metrics.height = size.height;
    fixedMetrics.metrics.left = 3;
    fixedMetrics.metrics.top = -1;
    fixedMetrics.metrics.advance = 24;

    PremultipliedImage rgbaBitmap = drawGlyphBitmap(glyphID, font, size);
   
    // Copy alpha values from RGBA bitmap into the AlphaImage output
    fixedMetrics.bitmap = AlphaImage(size);
    for (uint32_t i = 0; i < size.width * size.height; i++) {
        fixedMetrics.bitmap.data[i] = rgbaBitmap.data[4 * i + 3];
    }

    return fixedMetrics;
}

} // namespace mbgl
