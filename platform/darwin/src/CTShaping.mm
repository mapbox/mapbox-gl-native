#include <mbgl/text/shaping.hpp>
#import "CFHandle.hpp"

#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>

namespace mbgl {

using CGColorSpaceHandle = CFHandle<CGColorSpaceRef, CGColorSpaceRef, CGColorSpaceRelease>;
using CGContextHandle = CFHandle<CGContextRef, CGContextRef, CGContextRelease>;
using CFStringRefHandle = CFHandle<CFStringRef, CFTypeRef, CFRelease>;
using CFAttributedStringRefHandle = CFHandle<CFAttributedStringRef, CFTypeRef, CFRelease>;
using CFDictionaryRefHandle = CFHandle<CFDictionaryRef, CFTypeRef, CFRelease>;
using CTFontDescriptorRefHandle = CFHandle<CTFontDescriptorRef, CFTypeRef, CFRelease>;
using CTLineRefHandle = CFHandle<CTLineRef, CFTypeRef, CFRelease>;

void shapeLine(Shaping&, const std::u16string&) {
//    for (char16_t chr : line) {
//        auto it = glyphs.find(chr);
//        if (it == glyphs.end() || !it->second) {
//            continue;
//        }
//
//        const Glyph& glyph = **it->second;
//
//        if (writingMode == WritingModeType::Horizontal || !util::i18n::hasUprightVerticalOrientation(chr)) {
//            shaping.positionedGlyphs.emplace_back(chr, x, y, false);
//            x += glyph.metrics.advance + spacing;
//        } else {
//            shaping.positionedGlyphs.emplace_back(chr, x, 0, true);
//            x += verticalHeight + spacing;
//        }
//    }
    // TODO: Turn line into a CTRun, and for each glyph add an entry to shaping.positionedGlyphs

}

GlyphIDs getGlyphDependencies(const std::u16string& text) {
    GlyphIDs dependencies;

    NSDictionary *fontAttributes = @{
        (NSString *)kCTFontSizeAttribute: @24.0
    };

    CFStringRefHandle string(CFStringCreateWithCharacters(NULL, reinterpret_cast<UniChar*>(const_cast<char16_t*>(text.c_str())), text.length()));
    CFAttributedStringRef attrStr = CFAttributedStringCreate(NULL, *string, (CFDictionaryRef)fontAttributes);
    CTLineRef line = CTLineCreateWithAttributedString(attrStr);
    CFArrayRef runs = CTLineGetGlyphRuns(line);
    
     for (CFIndex i = 0; i < CFArrayGetCount(runs); i++) {
        CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runs, i);
        CFDictionaryRef attrs = CTRunGetAttributes(run);
        
        CTFontRef fontRef = (CTFontRef)CFDictionaryGetValue(attrs, CFSTR("NSFont"));
        CFStringRef fontNameRef = CTFontCopyName(fontRef, kCTFontFullNameKey);
        CFIndex length = CFStringGetLength(fontNameRef);
        UniChar* buffer = (UniChar*)malloc(length * sizeof(UniChar));
        CFStringGetCharacters(fontNameRef, CFRangeMake(0, length), buffer);
        std::u16string fontName((char16_t*)buffer, length);
        CFRelease(fontNameRef);
        free(buffer);

        const CGGlyph *runGlyphs = CTRunGetGlyphsPtr(run);
        if (runGlyphs) {
            for (NSUInteger j = 0; j < sizeof(runGlyphs) / sizeof(runGlyphs[0]); j++) {
                dependencies.insert(GlyphID(fontName, runGlyphs[j]));
            }
        }
    }
    return dependencies;
}

}; // end mbgl
