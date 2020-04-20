#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/constants.hpp>

#include <unordered_map>
#include <iterator>

#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>
#import <ImageIO/ImageIO.h>

#import "CFHandle.hpp"

/// Enables local glyph rasterization for all writing systems, not just CJK.
#define MBGL_DARWIN_NO_REMOTE_FONTS 1

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

CTFontDescriptorRef createFontDescriptor(const FontStack& fontStack, NSArray<NSString *>* fallbackFontNames, bool isVertical);

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
 @param size The size of the glyph.
 @returns An image containing the glyph.
 */
PremultipliedImage drawGlyphBitmap(GlyphID glyphID, CTFontRef font, Size size) {
    CGGlyph glyphs[] = { glyphID };
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
    
    CGContextSetShouldSubpixelPositionFonts(*context, false);
    CGContextSetShouldSubpixelQuantizeFonts(*context, false);
    
    CGPoint positions[1];
    positions[0] = CGPointMake(0.0, 0.0);
    CTFontDrawGlyphs(font, glyphs, positions, 1, *context);
    
    const CGFloat *black = CGColorGetComponents(CGColorGetConstantColor(kCGColorBlack));
    CGContextSetFillColor(*context, black);
    CGContextFillRect(*context, CGRectMake(0, 0, size.width, size.height));
    
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
    CFStringRef fontName = (__bridge CFStringRef)@(fontStack.front().c_str());
    CTFontRefHandle font(CTFontCreateWithName(fontName, 0.0, NULL));
    
    manufacturedGlyph.id = glyphID;

    // TODO: Plumb through vertical text.
    CTFontOrientation orientation = kCTFontOrientationHorizontal;
    
    CGRect boundingRects[1];
    CGGlyph glyphs[] = { glyphID };
    CGRect boundingRect = CTFontGetBoundingRectsForGlyphs(*font, orientation, glyphs, boundingRects, 1);
    if (CGRectIsNull(boundingRect)) {
        throw std::runtime_error("CTFontGetBoundingRectsForGlyphs failed");
    }
    manufacturedGlyph.metrics.left = std::round(CGRectGetMinX(boundingRects[0]));
    manufacturedGlyph.metrics.top = std::round(CGRectGetMinY(boundingRects[0]));
    // Mimic glyph PBF metrics.
    manufacturedGlyph.metrics.width = 35;
    manufacturedGlyph.metrics.height = 35;
    
    CGSize advances[1];
    CTFontGetAdvancesForGlyphs(*font, orientation, glyphs, advances, 1);
    manufacturedGlyph.metrics.advance = std::ceil(advances[0].width);
    
    Size size(MAX(manufacturedGlyph.metrics.width, 1), MAX(manufacturedGlyph.metrics.height, 1));
    PremultipliedImage rgbaBitmap = drawGlyphBitmap(glyphID, *font, size);
   
    // Copy alpha values from RGBA bitmap into the AlphaImage output
    manufacturedGlyph.bitmap = AlphaImage(size);
    for (uint32_t i = 0; i < size.width * size.height; i++) {
        manufacturedGlyph.bitmap.data[i] = rgbaBitmap.data[4 * i + 3];
    }

    return manufacturedGlyph;
}

/**
 Creates a font descriptor representing the given font stack and any global
 fallback fonts.
 
 @param fontStack The font stack that takes precedence.
 @param fallbackFontNames A list of font names to use as fallbacks if none of
    the fonts in the font stack is available.
 @param isVertical Whether the text to be drawn is laid out vertically.
 @returns A font descriptor representing the first font in the font stack with a
    cascade list representing the rest of the fonts in the font stack and any
    fallback fonts. The font descriptor is not cached.
 
 @post The caller is responsible for releasing the font descriptor.
 */
CTFontDescriptorRef createFontDescriptor(const FontStack& fontStack, NSArray<NSString *>* fallbackFontNames, bool isVertical) {
    NSMutableArray *fontNames = [NSMutableArray arrayWithCapacity:fontStack.size() + fallbackFontNames.count];
    for (auto& fontName : fontStack) {
        // Per the Mapbox Style Specification, the text-font property comes with
        // these last resort fonts by default, but they shouldn’t take
        // precedence over any application or system fallback font that may be
        // more appropriate to the current device.
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
    
    // Apply the first font name to the returned font descriptor; apply the rest
    // of the font names to the cascade list.
    CFStringRef mainFontName = (__bridge CFStringRef)fontNames.firstObject;
    CFMutableArrayRefHandle fallbackDescriptors(CFArrayCreateMutable(kCFAllocatorDefault, fontNames.count, &kCFTypeArrayCallBacks));
    for (NSString *name in fontNames) {
        NSDictionary *fontAttributes = @{
            (NSString *)kCTFontSizeAttribute: @(util::ONE_EM),
            // The name could be any of these three attributes of the font. It’s
            // OK if it doesn’t match all three; Core Text will pick the font
            // that matches the most attributes.
            (NSString *)kCTFontNameAttribute: name,
            (NSString *)kCTFontDisplayNameAttribute: name,
            (NSString *)kCTFontFamilyNameAttribute: name,
        };
        
        CTFontDescriptorRefHandle descriptor(CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes));
        CFArrayAppendValue(*fallbackDescriptors, *descriptor);
    }
    
    CTFontOrientation orientation = isVertical ? kCTFontOrientationVertical : kCTFontOrientationHorizontal;

    CFStringRef keys[] = {
        kCTFontSizeAttribute,
        kCTFontNameAttribute, kCTFontDisplayNameAttribute, kCTFontFamilyNameAttribute,
        kCTFontCascadeListAttribute,
        kCTFontOrientationAttribute,
    };
    CFTypeRef values[] = {
        (__bridge CFNumberRef)@(util::ONE_EM),
        mainFontName, mainFontName, mainFontName,
        *fallbackDescriptors,
        (__bridge CFNumberRef)@(orientation),
    };

    CFDictionaryRefHandle attributes(
        CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
            (const void**)&values, sizeof(keys) / sizeof(keys[0]),
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks));
    return CTFontDescriptorCreateWithAttributes(*attributes);
}

GlyphDependencies getGlyphDependencies(const FontStack& fontStack, const std::string& text, bool isVertical) {
    // TODO: Implement global fallback fonts.
    CTFontDescriptorRefHandle descriptor(createFontDescriptor(fontStack, @[], isVertical));
    CTFontRefHandle font(CTFontCreateWithFontDescriptor(*descriptor, 0.0, NULL));

    CFStringRef keys[] = { kCTFontAttributeName };
    CFTypeRef values[] = { *font };

    CFDictionaryRefHandle attributes(
        CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
            (const void**)&values, sizeof(keys) / sizeof(keys[0]),
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks));
    if (!attributes) {
        throw std::runtime_error("Unable to create attributed string attributes dictionary");
    }

    CFStringRef string = (__bridge CFStringRef)@(text.c_str());
    CFAttributedStringRefHandle attrString(CFAttributedStringCreate(kCFAllocatorDefault, string, *attributes));
    if (!attrString) {
        throw std::runtime_error("Unable to create attributed string");
    }
    CTLineRefHandle line(CTLineCreateWithAttributedString(*attrString));
    if (!line) {
        throw std::runtime_error("Unable to create line from attributed string");
    }
    
    CFArrayRef glyphRuns = CTLineGetGlyphRuns(*line);
    GlyphDependencies dependencies;
    for (CFIndex i = 0; i < CFArrayGetCount(glyphRuns); i++) {
        CTRunRef glyphRun = (CTRunRef)CFArrayGetValueAtIndex(glyphRuns, 0);
        CFRange wholeRunRange = CFRangeMake(0, CTRunGetGlyphCount(glyphRun));
        
        CTFontRef glyphFont = (CTFontRef)CFDictionaryGetValue(CTRunGetAttributes(glyphRun), kCTFontAttributeName);
        CFStringRefHandle glyphFontName(CTFontCopyName(glyphFont, kCTFontPostScriptNameKey));
        FontStack glyphFontStack = {{ [(__bridge NSString *)*glyphFontName UTF8String] }};
        
        // Use CTRunGetGlyphsPtr() if available.
        CGGlyph glyphs[wholeRunRange.length];
        CTRunGetGlyphs(glyphRun, wholeRunRange, glyphs);
        
        GlyphIDs& glyphIDs = dependencies[glyphFontStack];
        glyphIDs.insert(glyphs, glyphs + wholeRunRange.length);
    }
    return dependencies;
}

} // namespace mbgl
