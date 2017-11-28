#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>

#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>
#import <ImageIO/ImageIO.h>

namespace {

template <typename T, typename S, void (*Releaser)(S)>
struct CFHandle {
    CFHandle(T t_): t(t_) {}
    ~CFHandle() { Releaser(t); }
    T operator*() { return t; }
    operator bool() { return t; }
private:
    T t;
};

} // namespace


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

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack&, GlyphID glyphID) {
    // TODO: This is a rough approximation of the set of glyphs that will work with fixed glyph metrics
    // Either narrow this down to be conservative, or actually extract glyph metrics in rasterizeGlyph
    return util::i18n::allowsIdeographicBreaking(glyphID);
}

using CGContextHandle = CFHandle<CGContextRef, CGContextRef, CGContextRelease>;
using CGColorSpaceHandle = CFHandle<CGColorSpaceRef, CGColorSpaceRef, CGColorSpaceRelease>;

Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack&, GlyphID glyphID) {
    Glyph fixedMetrics;
    fixedMetrics.id = glyphID;

    uint32_t width = 35;
    uint32_t height = 35;
    
    fixedMetrics.metrics.width = width;
    fixedMetrics.metrics.height = height;
    fixedMetrics.metrics.left = 3;
    fixedMetrics.metrics.top = -1;
    fixedMetrics.metrics.advance = 24;

    Size size(width, height);

    fixedMetrics.bitmap = AlphaImage(size);
    
    NSDictionary *fontAttributes =
              [NSDictionary dictionaryWithObjectsAndKeys:
                      [NSNumber numberWithFloat:24.0], (NSString *)kCTFontSizeAttribute,
                      nil];
    // Create a descriptor.
    CTFontDescriptorRef descriptor =
              CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes);

    // Create a font using the descriptor.
    CTFontRef font = CTFontCreateWithFontDescriptor(descriptor, 0.0, NULL);
    CFRelease(descriptor);


    CFStringRef string = CFStringCreateWithCharacters(NULL, (UniChar*)&glyphID, 1);

    PremultipliedImage image(size);

    CGColorSpaceHandle colorSpace(CGColorSpaceCreateDeviceRGB());
    // TODO: Is there a way to just draw a single alpha channel instead of copying it out of an RGB image? Doesn't seem like the grayscale colorspace is what I'm looking for...
    if (!colorSpace) {
        throw std::runtime_error("CGColorSpaceCreateDeviceRGB failed");
    }
    
    constexpr const size_t bitsPerComponent = 8;
    constexpr const size_t bytesPerPixel = 4;
    const size_t bytesPerRow = bytesPerPixel * width;

    CGContextHandle context(CGBitmapContextCreate(
        image.data.get(),
        width,
        height,
        bitsPerComponent,
        bytesPerRow,
        *colorSpace,
        kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast));
    if (!context) {
        throw std::runtime_error("CGBitmapContextCreate failed");
    }

    CFStringRef keys[] = { kCTFontAttributeName };
    CFTypeRef values[] = { font };

    CFDictionaryRef attributes =
        CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
            (const void**)&values, sizeof(keys) / sizeof(keys[0]),
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks);

    CFAttributedStringRef attrString =
        CFAttributedStringCreate(kCFAllocatorDefault, string, attributes);
    CFRelease(string);
    CFRelease(attributes);

    CTLineRef line = CTLineCreateWithAttributedString(attrString); // TODO: Get glyph runs (for metric extraction) and use showGlyphs API instead?

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
    
    // Set text position and draw the line into the graphics context
    CGContextSetTextPosition(*context, 0.0, 5.0);
    CTLineDraw(line, *context);
    
    CFRelease(line);
    CFRelease(font);
    
    for (uint32_t i = 0; i < width * height; i++) {
        fixedMetrics.bitmap.data[i] = image.data[4 * i + 3]; // alpha value
    }

    return fixedMetrics;
}

} // namespace mbgl
