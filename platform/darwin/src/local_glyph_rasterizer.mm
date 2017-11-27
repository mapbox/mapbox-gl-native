#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>

#import <Foundation/Foundation.h>

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

    fixedMetrics.metrics.width = 24;
    fixedMetrics.metrics.height = 24;
    fixedMetrics.metrics.left = 0;
    fixedMetrics.metrics.top = -8;
    fixedMetrics.metrics.advance = 24;

    uint32_t width = 30;
    uint32_t height = 30;
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

    // Set text position and draw the line into the graphics context
    CGContextSetTextPosition(*context, 0.0, 0.0);
    CTLineDraw(line, *context);
    CFRelease(line);
    CFRelease(font);
    CFRelease(string); // TODO: Surely leaking something here, wrap these up!
    
    for (uint32_t i = 0; i < width * height; i++) {
        fixedMetrics.bitmap.data[i] = image.data[4 * i + 3]; // alpha value
    }

    return fixedMetrics;
}

} // namespace mbgl
