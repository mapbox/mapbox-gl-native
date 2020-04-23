#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/constants.hpp>

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
using CFMutableArrayRefHandle = CFHandle<CFMutableArrayRef, CFTypeRef, CFRelease>;
using CFDictionaryRefHandle = CFHandle<CFDictionaryRef, CFTypeRef, CFRelease>;
using CTFontRefHandle = CFHandle<CTFontRef, CFTypeRef, CFRelease>;
using CTFontDescriptorRefHandle = CFHandle<CTFontDescriptorRef, CFTypeRef, CFRelease>;
using CTLineRefHandle = CFHandle<CTLineRef, CFTypeRef, CFRelease>;

class LocalGlyphRasterizer::Impl {
public:
    Impl(const optional<std::string> fontFamily_)
    {
        fallbackFontNames = [[NSUserDefaults standardUserDefaults] stringArrayForKey:@"MGLIdeographicFontFamilyName"];
        if (fontFamily_) {
            fallbackFontNames = [fallbackFontNames ?: @[] arrayByAddingObjectsFromArray:[@(fontFamily_->c_str()) componentsSeparatedByString:@"\n"]];
        }
    }
    
    /**
     Returns whether local glyph rasterization is enabled globally.
     
     The developer can disable local glyph rasterization by specifying no
     fallback font names.
     */
    bool isEnabled() { return fallbackFontNames; }
    
    CTFontDescriptorRef createFontDescriptor(const FontStack& fontStack) {
        NSMutableArray *fontNames = [NSMutableArray arrayWithCapacity:fontStack.size() + fallbackFontNames.count];
        for (auto& fontName : fontStack) {
            // Per the Mapbox Style Specification, the text-font property comes
            // with these last resort fonts by default, but they shouldn’t take
            // precedence over any application or system fallback font that may
            // be more appropriate to the current device.
            if (fontName != util::LAST_RESORT_ALPHABETIC_FONT && fontName != util::LAST_RESORT_PAN_UNICODE_FONT) {
                [fontNames addObject:@(fontName.c_str())];
            }
        }
        [fontNames addObjectsFromArray:fallbackFontNames];
        
        if (!fontNames.count) {
            NSDictionary *fontAttributes = @{
                (NSString *)kCTFontSizeAttribute: @(util::ONE_EM),
            };
            return CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes);
        }
        
        // Apply the first font name to the returned font descriptor; apply the
        // rest of the font names to the cascade list.
        CFStringRef mainFontName = (__bridge CFStringRef)fontNames.firstObject;
        CFMutableArrayRefHandle fallbackDescriptors(CFArrayCreateMutable(kCFAllocatorDefault, fontNames.count, &kCFTypeArrayCallBacks));
        for (NSString *name in [fontNames subarrayWithRange:NSMakeRange(1, fontNames.count - 1)]) {
            NSDictionary *fontAttributes = @{
                (NSString *)kCTFontSizeAttribute: @(util::ONE_EM),
                // The name could be any of these three attributes of the font.
                // It’s OK if it doesn’t match all three; Core Text will pick
                // the font that matches the most attributes.
                (NSString *)kCTFontNameAttribute: name,
                (NSString *)kCTFontDisplayNameAttribute: name,
                (NSString *)kCTFontFamilyNameAttribute: name,
            };
            
            CTFontDescriptorRefHandle descriptor(CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes));
            CFArrayAppendValue(*fallbackDescriptors, *descriptor);
        }
        
        CFStringRef keys[] = {
            kCTFontSizeAttribute,
            kCTFontNameAttribute, kCTFontDisplayNameAttribute, kCTFontFamilyNameAttribute,
            kCTFontCascadeListAttribute,
        };
        CFTypeRef values[] = {
            (__bridge CFNumberRef)@(util::ONE_EM),
            mainFontName, mainFontName, mainFontName,
            *fallbackDescriptors,
        };

        CFDictionaryRefHandle attributes(
            CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
                (const void**)&values, sizeof(keys) / sizeof(keys[0]),
                &kCFTypeDictionaryKeyCallBacks,
                &kCFTypeDictionaryValueCallBacks));
        return CTFontDescriptorCreateWithAttributes(*attributes);
    }

    CTFontRef createFont(const FontStack& fontStack) {
        CTFontDescriptorRefHandle descriptor(createFontDescriptor(fontStack));
        return CTFontCreateWithFontDescriptor(*descriptor, 0.0, NULL);
    }
    
private:
    NSArray<NSString *> *fallbackFontNames;
};

LocalGlyphRasterizer::LocalGlyphRasterizer(const optional<std::string>& fontFamily)
    : impl(std::make_unique<Impl>(fontFamily))
{}

LocalGlyphRasterizer::~LocalGlyphRasterizer()
{}

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack&, GlyphID glyphID) {
    return util::i18n::allowsFixedWidthGlyphGeneration(glyphID) && impl->isEnabled();
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

Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack& fontStack, GlyphID glyphID) {
    Glyph fixedMetrics;
    CTFontRef font = impl->createFont(fontStack);
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
