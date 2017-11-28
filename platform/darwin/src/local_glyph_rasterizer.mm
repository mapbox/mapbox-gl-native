#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>

#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>
#import <ImageIO/ImageIO.h>

#import "CFHandle.hpp"

namespace mbgl {

/*
    Initial implementation of darwin TinySDF support:
    Draw any CJK glyphs using a default system font
 
    Where to take this:
     - Configure whether to use local fonts, and which fonts to use,
       based on map or even style layer options
     - Build heuristics for choosing fonts based on input FontStack
       (maybe a globally configurable FontStack -> UIFontDescriptor map would make sense?
     - Extract glyph metrics so that this can be used with more than just fixed width glyphs
*/

using CTFontDescriptorRefHandle = CFHandle<CTFontDescriptorRef, CFTypeRef, CFRelease>;
using CTLineRefHandle = CFHandle<CTLineRef, CFTypeRef, CFRelease>;

class LocalGlyphRasterizer::Impl {
public:
    Impl(CTFontRef font_)
        : font(font_)
    {}
    
    ~Impl() {
        if (font) {
            CFRelease(font);
        }
    }

    CTFontRef font;
};

LocalGlyphRasterizer::LocalGlyphRasterizer(const optional<std::string> fontFamily)
{
    if (fontFamily) {
        NSDictionary *fontAttributes = @{
            (NSString *)kCTFontSizeAttribute: [NSNumber numberWithFloat:24.0],
            (NSString *)kCTFontFamilyNameAttribute: [[NSString alloc] initWithCString:fontFamily->c_str() encoding:NSUTF8StringEncoding]
        };

        CTFontDescriptorRefHandle descriptor(CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes));

        impl = std::make_unique<Impl>(CTFontCreateWithFontDescriptor(*descriptor, 0.0, NULL));
    } else {
        impl = std::make_unique<Impl>((CTFontRef)NULL);
    }
}

LocalGlyphRasterizer::~LocalGlyphRasterizer()
{}

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack&, GlyphID glyphID) {
    // TODO: This is a rough approximation of the set of glyphs that will work with fixed glyph metrics
    // Either narrow this down to be conservative, or actually extract glyph metrics in rasterizeGlyph
    return impl->font && util::i18n::allowsIdeographicBreaking(glyphID);
}

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
    extractGlyphMetrics(font, *line);
    
    // Set text position and draw the line into the graphics context
    CGContextSetTextPosition(*context, 0.0, 5.0);
    CTLineDraw(*line, *context);
    
    return rgbaBitmap;
}

Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack&, GlyphID glyphID) {
    Glyph fixedMetrics;
    if (!impl->font) {
        return fixedMetrics;
    }
    
    fixedMetrics.id = glyphID;

    Size size(35, 35);
    
    fixedMetrics.metrics.width = size.width;
    fixedMetrics.metrics.height = size.height;
    fixedMetrics.metrics.left = 3;
    fixedMetrics.metrics.top = -1;
    fixedMetrics.metrics.advance = 24;

    PremultipliedImage rgbaBitmap = drawGlyphBitmap(glyphID, impl->font, size);
   
    // Copy alpha values from RGBA bitmap into the AlphaImage output
    fixedMetrics.bitmap = AlphaImage(size);
    for (uint32_t i = 0; i < size.width * size.height; i++) {
        fixedMetrics.bitmap.data[i] = rgbaBitmap.data[4 * i + 3];
    }

    return fixedMetrics;
}

} // namespace mbgl
