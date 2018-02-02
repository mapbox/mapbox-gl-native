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


#define PTR_OR_ARRAY(name) (name##Ptr ?: name)

float shapeLine(Shaping& shaping, const std::u16string& text, const float y) {
    float maxLineLength = 0;

    NSDictionary *fontAttributes = @{
        (NSString *)kCTFontSizeAttribute: @24.0
    };

    CFStringRefHandle string(CFStringCreateWithCharacters(NULL, reinterpret_cast<UniChar*>(const_cast<char16_t*>(text.c_str())), text.length()));
    CFAttributedStringRef attrStr = CFAttributedStringCreate(NULL, *string, (CFDictionaryRef)fontAttributes);
    CTLineRef line = CTLineCreateWithAttributedString(attrStr);
    CFArrayRef runs = CTLineGetGlyphRuns(line);
    
    CGSize runAdvance = CGSizeZero;
    
     for (CFIndex i = 0; i < CFArrayGetCount(runs); i++) {
        CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runs, i);
        CFDictionaryRef attrs = CTRunGetAttributes(run);
        
        CTFontRef fontRef = (CTFontRef)CFDictionaryGetValue(attrs, CFSTR("NSFont"));
        CFStringRef fontNameRef = CTFontCopyName(fontRef, kCTFontFamilyNameKey);
        CFIndex length = CFStringGetLength(fontNameRef);
        UniChar* buffer = (UniChar*)malloc(length * sizeof(UniChar));
        CFStringGetCharacters(fontNameRef, CFRangeMake(0, length), buffer);
        std::u16string fontName((char16_t*)buffer, length);
        CFRelease(fontNameRef);
        free(buffer);

        const CGGlyph *runGlyphsPtr = CTRunGetGlyphsPtr(run);
        CFIndex runGlyphCount = CTRunGetGlyphCount(run);
        CGGlyph *runGlyphs = NULL;
        if (!runGlyphsPtr) {
            runGlyphs = (CGGlyph*)malloc(runGlyphCount * sizeof(CGGlyph));
            CTRunGetGlyphs(run, CFRangeMake(0, 0), runGlyphs);
        }

        NSDictionary* nsAttrs = (__bridge NSDictionary*)attrs;
        CTFontRef runFont = CTFontCreateWithName(fontNameRef, [nsAttrs[@"NSFontSizeAttribute"] doubleValue], NULL);

        CGRect boundingRects[runGlyphCount];
        CTFontGetBoundingRectsForGlyphs(runFont, kCTFontOrientationHorizontal, PTR_OR_ARRAY(runGlyphs), boundingRects, runGlyphCount);
        CGSize advances[runGlyphCount];
        CTFontGetAdvancesForGlyphs(runFont, kCTFontOrientationHorizontal, PTR_OR_ARRAY(runGlyphs), advances, runGlyphCount);
        //CGSize runAdvance = CGSizeZero;
        for (CFIndex j = 0; j < runGlyphCount; j++) {
            CGRect frame = boundingRects[j];
            frame.origin.x += runAdvance.width;
            //frame.origin.y += runAdvance.height;
            runAdvance.width += advances[j].width;
            runAdvance.height += advances[j].height;
            shaping.positionedGlyphs.emplace_back(GlyphID(fontName, PTR_OR_ARRAY(runGlyphs)[j]), frame.origin.x, y + frame.origin.y - frame.size.height, false);
            maxLineLength = std::max<float>(maxLineLength, frame.origin.x + frame.size.width);
        }
        free(runGlyphs);

    }
    return maxLineLength;
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
        CFStringRef fontNameRef = CTFontCopyName(fontRef, kCTFontFamilyNameKey);
        CFIndex length = CFStringGetLength(fontNameRef);
        UniChar* buffer = (UniChar*)malloc(length * sizeof(UniChar));
        CFStringGetCharacters(fontNameRef, CFRangeMake(0, length), buffer);
        std::u16string fontName((char16_t*)buffer, length);
        CFRelease(fontNameRef);
        free(buffer);

        const CGGlyph *runGlyphsPtr = CTRunGetGlyphsPtr(run);
        CFIndex runGlyphCount = CTRunGetGlyphCount(run);
        CGGlyph *runGlyphs = NULL;
        if (!runGlyphsPtr) {
            runGlyphs = (CGGlyph*)malloc(runGlyphCount * sizeof(CGGlyph));
            CTRunGetGlyphs(run, CFRangeMake(0, 0), runGlyphs);
        }

        for (CFIndex j = 0; j < runGlyphCount; j++) {
            dependencies.insert(GlyphID(fontName, PTR_OR_ARRAY(runGlyphs)[j]));
        }
        free(runGlyphs);
    }
    return dependencies;
}

}; // end mbgl
