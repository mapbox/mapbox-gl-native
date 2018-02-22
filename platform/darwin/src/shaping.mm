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

NSDictionary *getAttributes(const FontStack& fontStack) {
    NSDictionary *baseFontAttributes = @{
        (NSString *)kCTFontNameAttribute: @(fontStack.front().c_str()),
    };
    CTFontDescriptorRef baseDescriptor = CTFontDescriptorCreateWithAttributes((CFDictionaryRef)baseFontAttributes);
//    CTFontRef baseFont = CTFontCreateWithFontDescriptor(baseDescriptor, 0.0, NULL);
    CTFontRef font = CTFontCreateWithFontDescriptor(baseDescriptor, 0.0, NULL);
    return @{
        (NSString *)kCTFontSizeAttribute: @24.0,
        (NSString *)kCTFontAttributeName: (__bridge NSFont *)font,
        // TODO: Fall back to the rest of the font stack, then the default cascade list.
//        (NSString *)kCTFontCascadeListAttribute: (__bridge NSArray *)CTFontCopyDefaultCascadeListForLanguages(baseFont, NULL),
    };
}

float shapeLine(Shaping& shaping, const std::u16string& text, const float y, const FontStack& fontStack) {
    float maxLineLength = 0;

    NSDictionary *attrs = getAttributes(fontStack);

    CFStringRefHandle string(CFStringCreateWithCharacters(NULL, reinterpret_cast<UniChar*>(const_cast<char16_t*>(text.c_str())), text.length()));
    CFAttributedStringRef attrStr = CFAttributedStringCreate(NULL, *string, (CFDictionaryRef)attrs);
    CTLineRef line = CTLineCreateWithAttributedString(attrStr);
    CFArrayRef runs = CTLineGetGlyphRuns(line);
    
    CGSize runAdvance = CGSizeZero;
    
    for (CFIndex i = 0; i < CFArrayGetCount(runs); i++) {
        CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runs, i);
        CFDictionaryRef runAttrs = CTRunGetAttributes(run);
        
        CTFontRef fontRef = (CTFontRef)CFDictionaryGetValue(runAttrs, CFSTR("NSFont"));
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

        NSDictionary* nsAttrs = (__bridge NSDictionary*)runAttrs;
        CTFontRef runFont = CTFontCreateWithName(fontNameRef, [nsAttrs[@"NSFontSizeAttribute"] doubleValue], NULL);

        CGRect boundingRects[runGlyphCount];
        CTFontGetBoundingRectsForGlyphs(runFont, kCTFontOrientationHorizontal, PTR_OR_ARRAY(runGlyphs), boundingRects, runGlyphCount);
        CGSize advances[runGlyphCount];
        CTFontGetAdvancesForGlyphs(runFont, kCTFontOrientationHorizontal, PTR_OR_ARRAY(runGlyphs), advances, runGlyphCount);
        //CGSize runAdvance = CGSizeZero;
        for (CFIndex j = 0; j < runGlyphCount; j++) {
            CGRect frame = boundingRects[j];
            frame.origin.x += runAdvance.width;
            frame.origin.y += runAdvance.height;
            shaping.positionedGlyphs.emplace_back(GlyphID(fontName, PTR_OR_ARRAY(runGlyphs)[j]), runAdvance.width, y - CGRectGetHeight(frame), false);
            runAdvance.width += advances[j].width;
            runAdvance.height += advances[j].height;
            maxLineLength = std::max<float>(maxLineLength, CGRectGetMaxX(frame));
        }
        free(runGlyphs);

    }
    return maxLineLength;
}

GlyphIDs getGlyphDependencies(const FontStack& fontStack, const std::u16string& text) {
    GlyphIDs dependencies;

    NSDictionary *attrs = getAttributes(fontStack);

    CFStringRefHandle string(CFStringCreateWithCharacters(NULL, reinterpret_cast<UniChar*>(const_cast<char16_t*>(text.c_str())), text.length()));
    CFAttributedStringRef attrStr = CFAttributedStringCreate(NULL, *string, (CFDictionaryRef)attrs);
    CTLineRef line = CTLineCreateWithAttributedString(attrStr);
    CFArrayRef runs = CTLineGetGlyphRuns(line);
    
    for (CFIndex i = 0; i < CFArrayGetCount(runs); i++) {
        CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runs, i);
        CFDictionaryRef runAttrs = CTRunGetAttributes(run);
        
        CTFontRef fontRef = (CTFontRef)CFDictionaryGetValue(runAttrs, CFSTR("NSFont"));
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
