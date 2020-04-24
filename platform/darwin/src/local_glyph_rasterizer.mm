#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/constants.hpp>

#include <unordered_map>

#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>
#import <ImageIO/ImageIO.h>

#import "CFHandle.hpp"

/// Enables local glyph rasterization for all writing systems, not just CJK.
#define MBGL_DARWIN_NO_REMOTE_FONTS 0

namespace mbgl {

using CGColorSpaceHandle = CFHandle<CGColorSpaceRef, CGColorSpaceRef, CGColorSpaceRelease>;
using CGContextHandle = CFHandle<CGContextRef, CGContextRef, CGContextRelease>;
using CFStringRefHandle = CFHandle<CFStringRef, CFTypeRef, CFRelease>;
using CFAttributedStringRefHandle = CFHandle<CFAttributedStringRef, CFTypeRef, CFRelease>;
using CFMutableArrayRefHandle = CFHandle<CFMutableArrayRef, CFTypeRef, CFRelease>;
using CFDictionaryRefHandle = CFHandle<CFDictionaryRef, CFTypeRef, CFRelease>;
using CTFontRefHandle = CFHandle<CTFontRef, CFTypeRef, CFRelease>;
using CTFontDescriptorRefHandle = CFHandle<CTFontDescriptorRef, CFTypeRef, CFRelease>;
using CTLineRefHandle = CFHandle<CTLineRef, CFTypeRef, CFRelease>;

/**
 Draws glyphs applying fonts that are installed on the system or bundled with
 the application. This is a more flexible and performant alternative to
 typesetting text using glyph sheets downloaded from a server.
 
 This implementation is similar to the local glyph rasterization in GL JS:
  - Only CJK glyphs are drawn locally, because it’s much more noticeable when a
    non-CJK font mismatches the style than when a CJK font mismatches the style.
    (Unlike GL JS, this implementation does respect the font’s metrics, so it
    does work with variable-width fonts.)
  - Fallback fonts can be specified globally, similar to the seldom-used
    advanced font preferences in most Web browsers.
 
 This is a first step toward fully local font rendering:
 <https://github.com/mapbox/mapbox-gl-native/issues/7862>. Further improvements:
  - Make sure the font size is 24 points (`util::ONE_EM`) at all times, not the
    system default of 12 points, to avoid blobbiness after rasterization.
  - Sniff an appropriate font weight and style from the font stack’s font names,
    as GL JS and the Android map SDK do.
  - Enable local glyph rasterization for all writing systems, not just CJK. This
    would require providing a more attractive default Latin font than Helvetica
    or Arial Unicode MS.
  - Allow the developer to specify a `CTFontDescriptor` or
    `NSFontDescriptor`/`UIFontDescriptor` to customize more attributes besides
    the font (but not the font size).
  - Render glyphs directly to SDF using Core Text’s `CTFontCreatePathForGlyph()`
    instead of going through TinySDF.
  - Typeset an entire text label in one shot using `CTFramesetter` to take
    advantage of Core Text’s superior complex text shaping capabilities and
    support for astral plane characters. mbgl would need to stop conflating
    codepoints with glyph IDs.
*/
class LocalGlyphRasterizer::Impl {
public:
    /**
     Creates a new rasterizer with the given font names as a fallback.
     
     The fallback font names can also be specified in the style as a font stack
     or in the `MGLIdeographicFontFamilyName` key of
     `NSUserDefaults.standardUserDefaults`. The font stack takes precedence,
     followed by the `MGLIdeographicFontFamilyName` user default, then finally
     the `fallbackFontNames_` parameter as a last resort.
     
     @param fallbackFontNames_ A list of font names, one per line. Each font
        name can be the PostScript name or display name of a specific font face
        or a font family name. Set this parameter to `nullptr` to disable local
        glyph rasterization globally. The system font is a good default value to
        pass into this constructor.
     */
    Impl(const optional<std::string> fallbackFontNames_)
    {
        fallbackFontNames = [[NSUserDefaults standardUserDefaults] stringArrayForKey:@"MGLIdeographicFontFamilyName"];
        if (fallbackFontNames_) {
            fallbackFontNames = [fallbackFontNames ?: @[] arrayByAddingObjectsFromArray:[@(fallbackFontNames_->c_str()) componentsSeparatedByString:@"\n"]];
        }
    }
    
    /**
     Returns whether local glyph rasterization is enabled globally.
     
     The developer can disable local glyph rasterization by specifying no
     fallback font names.
     */
    bool isEnabled() { return fallbackFontNames; }
    
    /**
     Creates a font descriptor representing the given font stack and any global
     fallback fonts.
     
     @param fontStack The font stack that takes precedence.
     @returns A font descriptor representing the first font in the font stack
        with a cascade list representing the rest of the fonts in the font stack
        and any fallback fonts. The font descriptor is not cached.
     
     @post The caller is responsible for releasing the font descriptor.
     */
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
                // This attribute is technically supposed to be a font’s
                // PostScript name, but Core Text will fall back to matching
                // font display names and font family names.
                (NSString *)kCTFontNameAttribute: name,
            };
            
            CTFontDescriptorRefHandle descriptor(CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes));
            CFArrayAppendValue(*fallbackDescriptors, *descriptor);
        }
        
        CFStringRef keys[] = {
            kCTFontSizeAttribute,
            kCTFontNameAttribute,
            kCTFontCascadeListAttribute,
        };
        CFTypeRef values[] = {
            (__bridge CFNumberRef)@(util::ONE_EM),
            mainFontName,
            *fallbackDescriptors,
        };

        CFDictionaryRefHandle attributes(
            CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
                (const void**)&values, sizeof(keys) / sizeof(keys[0]),
                &kCFTypeDictionaryKeyCallBacks,
                &kCFTypeDictionaryValueCallBacks));
        return CTFontDescriptorCreateWithAttributes(*attributes);
    }
    
private:
    NSArray<NSString *> *fallbackFontNames;
};

LocalGlyphRasterizer::LocalGlyphRasterizer(const optional<std::string>& fontFamily)
    : impl(std::make_unique<Impl>(fontFamily))
{}

LocalGlyphRasterizer::~LocalGlyphRasterizer()
{}

/**
 Returns whether the rasterizer can rasterize a glyph for the given codepoint.
 
 @param glyphID A font-agnostic Unicode codepoint, not a glyph index.
 @returns Whether a glyph for the codepoint can be rasterized.
 */
bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack&, GlyphID glyphID) {
#if MBGL_DARWIN_NO_REMOTE_FONTS
    return impl->isEnabled();
#else
    return util::i18n::allowsFixedWidthGlyphGeneration(glyphID) && impl->isEnabled();
#endif
}

/**
 Draws the given codepoint into an image, gathers metrics about the glyph, and
 returns the image.
 
 @param glyphID A font-agnostic Unicode codepoint, not a glyph index.
 @param font The font to apply to the codepoint.
 @param metrics Upon return, the metrics match the font’s metrics for the glyph
    representing the codepoint.
 @returns An image containing the glyph.
 */
PremultipliedImage drawGlyphBitmap(GlyphID glyphID, CTFontRef font, GlyphMetrics& metrics) {
    CFStringRefHandle string(CFStringCreateWithCharacters(NULL, reinterpret_cast<UniChar*>(&glyphID), 1));
    if (!string) {
        throw std::runtime_error("Unable to create string from codepoint");
    }

    CFStringRef keys[] = { kCTFontAttributeName };
    CFTypeRef values[] = { font };

    CFDictionaryRefHandle attributes(
        CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
            (const void**)&values, sizeof(keys) / sizeof(keys[0]),
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks));
    if (!attributes) {
        throw std::runtime_error("Unable to create attributed string attributes dictionary");
    }

    CFAttributedStringRefHandle attrString(CFAttributedStringCreate(kCFAllocatorDefault, *string, *attributes));
    if (!attrString) {
        throw std::runtime_error("Unable to create attributed string");
    }
    CTLineRefHandle line(CTLineCreateWithAttributedString(*attrString));
    if (!line) {
        throw std::runtime_error("Unable to create line from attributed string");
    }
    
    Size size(35, 35);
    metrics.width = size.width;
    metrics.height = size.height;
    
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
    
    CFArrayRef glyphRuns = CTLineGetGlyphRuns(*line);
    CTRunRef glyphRun = (CTRunRef)CFArrayGetValueAtIndex(glyphRuns, 0);
    CFRange wholeRunRange = CFRangeMake(0, CTRunGetGlyphCount(glyphRun));
    CGSize advances[wholeRunRange.length];
    CTRunGetAdvances(glyphRun, wholeRunRange, advances);
    metrics.advance = std::round(advances[0].width);
    
    // Mimic glyph PBF metrics.
    metrics.left = Glyph::borderSize;
    metrics.top = -1;
    
    // Move the text upward to avoid clipping off descenders.
    CGFloat descent;
    CTRunGetTypographicBounds(glyphRun, wholeRunRange, NULL, &descent, NULL);
    CGContextSetTextPosition(*context, 0.0, descent);
    
    CTLineDraw(*line, *context);
    
    return rgbaBitmap;
}

/**
 Returns all the information mbgl needs about a glyph representation of the
 given codepoint.
 
 @param fontStack The best matching font in this font stack is applied to the
    codepoint.
 @param glyphID A font-agnostic Unicode codepoint, not a glyph index.
 @returns A glyph representation of the given codepoint with its bitmap data and
    metrics set.
 */
Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack& fontStack, GlyphID glyphID) {
    Glyph manufacturedGlyph;
    CTFontDescriptorRefHandle descriptor(impl->createFontDescriptor(fontStack));
    CTFontRefHandle font(CTFontCreateWithFontDescriptor(*descriptor, 0.0, NULL));
    if (!font) {
        return manufacturedGlyph;
    }
    
    manufacturedGlyph.id = glyphID;

    PremultipliedImage rgbaBitmap = drawGlyphBitmap(glyphID, *font, manufacturedGlyph.metrics);
    
    Size size(manufacturedGlyph.metrics.width, manufacturedGlyph.metrics.height);
    // Copy alpha values from RGBA bitmap into the AlphaImage output
    manufacturedGlyph.bitmap = AlphaImage(size);
    for (uint32_t i = 0; i < size.width * size.height; i++) {
        manufacturedGlyph.bitmap.data[i] = rgbaBitmap.data[4 * i + 3];
    }

    return manufacturedGlyph;
}

} // namespace mbgl
