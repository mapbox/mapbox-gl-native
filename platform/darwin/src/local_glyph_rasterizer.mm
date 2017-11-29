#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>

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
     - We use heuristics to extract a font-weight based on the incoming font stack
 
    Further improvements are possible:
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

    
    CTFontRef getFont(const FontStack& fontStack) {
        if (!fontFamily) {
            return NULL;
        }
        
        if (fontHandles.find(fontStack) == fontHandles.end()) {
            NSDictionary *fontAttributes = @{
                (NSString *)kCTFontSizeAttribute: [NSNumber numberWithFloat:24.0],
                (NSString *)kCTFontFamilyNameAttribute: [[NSString alloc] initWithCString:fontFamily->c_str() encoding:NSUTF8StringEncoding]
            };

            CTFontDescriptorRefHandle descriptor(CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes));
            CTFontRef font = CTFontCreateWithFontDescriptor(*descriptor, 0.0, NULL);
            if (!font) {
                throw std::runtime_error("CTFontCreateWithFontDescriptor failed");
            }

            fontHandles[fontStack] = font;
        }
        return fontHandles[fontStack];
    }
    
private:
    std::unordered_map<FontStack, CTFontRef, FontStackHash> fontHandles;
    optional<std::string> fontFamily;
};

LocalGlyphRasterizer::LocalGlyphRasterizer(const optional<std::string> fontFamily)
    : impl(std::make_unique<Impl>(fontFamily))
{}

LocalGlyphRasterizer::~LocalGlyphRasterizer()
{}

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack& fontStack, GlyphID glyphID) {
    return util::i18n::allowsFixedWidthGlyphGeneration(glyphID) && impl->getFont(fontStack);
}

/*
// TODO: In theory we should be able to transform user-coordinate bounding box and advance
// values into pixel glyph metrics. This would remove the need to use fixed glyph metrics
// (which will be slightly off depending on the font), and allow us to return non CJK glyphs
// (which will have variable "advance" values).
void extractGlyphMetrics(CTFontRef font, CTLineRef line) {
    CFArrayRef glyphRuns = CTLineGetGlyphRuns(line);
    CFIndex runCount = CFArrayGetCount(glyphRuns);
    assert(runCount == 1);
    CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(glyphRuns, 0);
    CFIndex glyphCount = CTRunGetGlyphCount(run);
    assert(glyphCount == 1);
    const CGGlyph *glyphs = CTRunGetGlyphsPtr(run);

    CGRect boundingRects[1];
    boundingRects[0] = CGRectMake(0, 0, 0, 0);
    CGSize advances[1];
    advances[0] = CGSizeMake(0,0);
    CGRect totalBoundingRect = CTFontGetBoundingRectsForGlyphs(font, kCTFontOrientationDefault, glyphs, boundingRects, 1);
    double totalAdvance = CTFontGetAdvancesForGlyphs(font, kCTFontOrientationDefault, glyphs, advances, 1);
    
    // Break in the debugger to see these values: translating from "user coordinates" to bitmap pixel coordinates
    // should be OK, but a lot of glyphs seem to have empty bounding boxes...?
    (void)totalBoundingRect;
    (void)totalAdvance;
}
*/

PremultipliedImage drawGlyphBitmap(GlyphID glyphID, CTFontRef font, Size size) {
    PremultipliedImage rgbaBitmap(size);
    
    CFStringRefHandle string(CFStringCreateWithCharacters(NULL, reinterpret_cast<UniChar*>(&glyphID), 1));

    CGColorSpaceHandle colorSpace(CGColorSpaceCreateDeviceRGB());
    // TODO: Is there a way to just draw a single alpha channel instead of copying it out of an RGB image? Doesn't seem like the grayscale colorspace is what I'm looking for...
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
    
    // For debugging only, doesn't get useful metrics yet
    //extractGlyphMetrics(font, *line);
    
    // Start drawing a little bit below the top of the bitmap
    CGContextSetTextPosition(*context, 0.0, 5.0);
    CTLineDraw(*line, *context);
    
    return rgbaBitmap;
}

Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack& fontStack, GlyphID glyphID) {
    Glyph fixedMetrics;
    CTFontRef font = impl->getFont(fontStack);
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
