#include <mbgl/text/shaping.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/layout/symbol_feature.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/text/bidi.hpp>
#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/utf.hpp>

#include <algorithm>
#include <list>
#include <cmath>

#import "CFHandle.hpp"

#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>

using CFAttributedStringRefHandle = CFHandle<CFAttributedStringRef, CFTypeRef, CFRelease>;
using CFDictionaryRefHandle = CFHandle<CFDictionaryRef, CFTypeRef, CFRelease>;
using CFMutableAttributedStringRefHandle = CFHandle<CFMutableAttributedStringRef, CFTypeRef, CFRelease>;
using CGPathRefHandle = CFHandle<CGPathRef, CGPathRef, CGPathRelease>;
using CTFontRefHandle = CFHandle<CTFontRef, CFTypeRef, CFRelease>;
using CTFontDescriptorRefHandle = CFHandle<CTFontDescriptorRef, CFTypeRef, CFRelease>;
using CTParagraphStyleRefHandle = CFHandle<CTParagraphStyleRef, CFTypeRef, CFRelease>;
using CTFrameRefHandle = CFHandle<CTFrameRef, CFTypeRef, CFRelease>;
using CTFramesetterRefHandle = CFHandle<CTFramesetterRef, CFTypeRef, CFRelease>;

namespace mbgl {

// static
AnchorAlignment AnchorAlignment::getAnchorAlignment(style::SymbolAnchorType anchor) {
    AnchorAlignment result(0.5f, 0.5f);

    switch (anchor) {
    case style::SymbolAnchorType::Right:
    case style::SymbolAnchorType::TopRight:
    case style::SymbolAnchorType::BottomRight:
        result.horizontalAlign = 1.0f;
        break;
    case style::SymbolAnchorType::Left:
    case style::SymbolAnchorType::TopLeft:
    case style::SymbolAnchorType::BottomLeft:
        result.horizontalAlign = 0.0f;
        break;
    default:
        break;
    }

    switch (anchor) {
    case style::SymbolAnchorType::Bottom:
    case style::SymbolAnchorType::BottomLeft:
    case style::SymbolAnchorType::BottomRight:
        result.verticalAlign = 1.0f;
        break;
    case style::SymbolAnchorType::Top:
    case style::SymbolAnchorType::TopLeft:
    case style::SymbolAnchorType::TopRight:
        result.verticalAlign = 0.0f;
        break;
    default:
        break;
    }

    return result;
}

style::TextJustifyType getAnchorJustification(style::SymbolAnchorType anchor) {
    switch (anchor) {
    case style::SymbolAnchorType::Right:
    case style::SymbolAnchorType::TopRight:
    case style::SymbolAnchorType::BottomRight:
        return style::TextJustifyType::Right;
    case style::SymbolAnchorType::Left:
    case style::SymbolAnchorType::TopLeft:
    case style::SymbolAnchorType::BottomLeft:
        return style::TextJustifyType::Left;
    default:
        return style::TextJustifyType::Center;
    }
}

PositionedIcon PositionedIcon::shapeIcon(const ImagePosition& image,
                                         const std::array<float, 2>& iconOffset,
                                         style::SymbolAnchorType iconAnchor) {
    AnchorAlignment anchorAlign = AnchorAlignment::getAnchorAlignment(iconAnchor);
    float dx = iconOffset[0];
    float dy = iconOffset[1];
    float left = dx - image.displaySize()[0] * anchorAlign.horizontalAlign;
    float right = left + image.displaySize()[0];
    float top = dy - image.displaySize()[1] * anchorAlign.verticalAlign;
    float bottom = top + image.displaySize()[1];

    Padding collisionPadding;
    if (image.content) {
        auto& content = *image.content;
        const auto pixelRatio = image.pixelRatio;
        collisionPadding.left = content.left / pixelRatio;
        collisionPadding.top = content.top / pixelRatio;
        collisionPadding.right = image.displaySize()[0] - content.right / pixelRatio;
        collisionPadding.bottom = image.displaySize()[1] - content.bottom / pixelRatio;
    }

    return PositionedIcon{image, top, bottom, left, right, collisionPadding};
}

void PositionedIcon::fitIconToText(const Shaping& shapedText,
                                   const style::IconTextFitType textFit,
                                   const std::array<float, 4>& padding,
                                   const std::array<float, 2>& iconOffset,
                                   const float fontScale) {
    assert(textFit != style::IconTextFitType::None);
    assert(shapedText);

    // We don't respect the icon-anchor, because icon-text-fit is set. Instead,
    // the icon will be centered on the text, then stretched in the given
    // dimensions.

    const float textLeft = shapedText.left * fontScale;
    const float textRight = shapedText.right * fontScale;

    if (textFit == style::IconTextFitType::Width || textFit == style::IconTextFitType::Both) {
        // Stretched horizontally to the text width
        _left = iconOffset[0] + textLeft - padding[3];
        _right = iconOffset[0] + textRight + padding[1];
    } else {
        // Centered on the text
        _left = iconOffset[0] + (textLeft + textRight - image().displaySize()[0]) / 2.0f;
        _right = _left + image().displaySize()[0];
    }

    const float textTop = shapedText.top * fontScale;
    const float textBottom = shapedText.bottom * fontScale;
    if (textFit == style::IconTextFitType::Height || textFit == style::IconTextFitType::Both) {
        // Stretched vertically to the text height
        _top = iconOffset[1] + textTop - padding[0];
        _bottom = iconOffset[1] + textBottom + padding[2];
    } else {
        // Centered on the text
        _top = iconOffset[1] + (textTop + textBottom - image().displaySize()[1]) / 2.0f;
        _bottom = _top + image().displaySize()[1];
    }
}

// Defined in local_glyph_rasterizer.mm
CTFontDescriptorRef createFontDescriptor(const FontStack& fontStack, NSArray<NSString *>* fallbackFontNames, bool isVertical);

Shaping getShaping(const TaggedString& formattedString,
                   const float maxWidth,
                   const float lineHeight,
                   const style::SymbolAnchorType textAnchor,
                   const style::TextJustifyType textJustify,
                   const float spacing,
                   const std::array<float, 2>& translate,
                   const WritingModeType writingMode,
                   BiDi& bidi,
                   const GlyphMap& glyphMap,
                   const GlyphPositions& glyphPositions,
                   const ImagePositions& imagePositions,
                   float layoutTextSize,
                   float layoutTextSizeAtBucketZoomLevel,
                   bool allowVerticalPlacement) {
    // TODO: Vertical text.
//    const bool vertical = writingMode != WritingModeType::Horizontal && allowVerticalPlacement;
    const bool vertical = false;
    CTFontOrientation orientation = vertical ? kCTFontOrientationVertical : kCTFontOrientationHorizontal;
    
    CTTextAlignment textAlignment;
    switch (textJustify) {
        case style::TextJustifyType::Auto:
            textAlignment = kCTTextAlignmentNatural;
            break;
        case style::TextJustifyType::Center:
            textAlignment = kCTTextAlignmentCenter;
            break;
        case style::TextJustifyType::Left:
            textAlignment = kCTTextAlignmentLeft;
            break;
        case style::TextJustifyType::Right:
            textAlignment = kCTTextAlignmentRight;
            break;
    }
    CGFloat lineHeightInEms = lineHeight / util::ONE_EM;
    CTParagraphStyleSetting paragraphSettings[] = {
        { kCTParagraphStyleSpecifierAlignment, sizeof(CTTextAlignment), &textAlignment },
        { kCTParagraphStyleSpecifierLineHeightMultiple, sizeof(float), &lineHeightInEms },
    };
    CTParagraphStyleRefHandle paragraphStyle(CTParagraphStyleCreate(paragraphSettings, sizeof(paragraphSettings) / sizeof(paragraphSettings[0])));
    
    CFStringRef keys[] = { kCTParagraphStyleAttributeName };
    CFTypeRef values[] = { *paragraphStyle };
    
    CFDictionaryRefHandle attributes(
         CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
                            (const void**)&values, sizeof(keys) / sizeof(keys[0]),
                            &kCFTypeDictionaryKeyCallBacks,
                            &kCFTypeDictionaryValueCallBacks));
    const auto& string = util::convertUTF16ToUTF8(formattedString.rawText());
    CFAttributedStringRefHandle attrString(CFAttributedStringCreate(kCFAllocatorDefault, (CFStringRef)@(string.c_str()), *attributes));
    CFMutableAttributedStringRefHandle mutableAttrString(CFAttributedStringCreateMutableCopy(kCFAllocatorDefault, 0, *attrString));
    for (std::size_t i = 0; i < formattedString.length(); i++) {
        const auto& section = formattedString.getSection(i);
        if (section.imageID) continue;

        // TODO: Coalesce attributes across entire runs, because TaggedString has already chopped up all the attribute runs by character.
        CTFontDescriptorRefHandle descriptor(createFontDescriptor(section.fontStack, @[], allowVerticalPlacement));
        CTFontRefHandle font(CTFontCreateWithFontDescriptor(*descriptor, 0.0, NULL));
        CFAttributedStringSetAttribute(*mutableAttrString, CFRangeMake(i, 1), kCTFontAttributeName, *font);
    }
    
    Shaping shaping(translate[0], translate[1], writingMode);
    shaping.verticalizable = true;
    
    CTFramesetterRefHandle framesetter(CTFramesetterCreateWithAttributedString(*mutableAttrString));
    CFRange fittingRange;
    CGSize suggestedSize = CTFramesetterSuggestFrameSizeWithConstraints(*framesetter, CFRangeMake(0, 0), NULL, CGSizeMake(maxWidth, CGFLOAT_MAX), &fittingRange);
    shaping.top = 0;
    shaping.left = 0;
    shaping.bottom = shaping.top + suggestedSize.height;
    shaping.right = shaping.left + suggestedSize.width;
    
    CGPathRefHandle path(CGPathCreateWithRect(CGRectMake(0, 0, suggestedSize.width, suggestedSize.height), NULL));
    CTFrameRefHandle frame(CTFramesetterCreateFrame(*framesetter, fittingRange, *path, NULL));
    CFArrayRef lines = CTFrameGetLines(*frame);
    for (CFIndex i = 0; i < CFArrayGetCount(lines); i++) {
        CTLineRef line = (CTLineRef)CFArrayGetValueAtIndex(lines, i);
        shaping.positionedLines.emplace_back();
        auto& positionedLine = shaping.positionedLines.back();
        
        CGRect bounds = CTLineGetBoundsWithOptions(line, 0);
        positionedLine.lineOffset = CGRectGetHeight(bounds);
        
        CFArrayRef runs = CTLineGetGlyphRuns(line);
        for (CFIndex j = 0; j < CFArrayGetCount(runs); j++) {
            CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runs, j);
            CFRange wholeRunRange = CFRangeMake(0, CTRunGetGlyphCount(run));
            
            CGGlyph glyphs[wholeRunRange.length];
            CTRunGetGlyphs(run, wholeRunRange, glyphs);
            
            CGPoint positions[wholeRunRange.length];
            CTRunGetPositions(run, wholeRunRange, positions);
            
            CTFontRef glyphFont = (CTFontRef)CFDictionaryGetValue(CTRunGetAttributes(run), kCTFontAttributeName);
            CGRect boundingRects[wholeRunRange.length];
            CTFontGetBoundingRectsForGlyphs(glyphFont, orientation, glyphs, boundingRects, wholeRunRange.length);
            
            CGSize advances[wholeRunRange.length];
            CTFontGetAdvancesForGlyphs(glyphFont, orientation, glyphs, advances, wholeRunRange.length);
            
            CFIndex stringIndices[CTRunGetStringRange(run).length];
            CTRunGetStringIndices(run, wholeRunRange, stringIndices);
            
            for (CFIndex k = 0; k < wholeRunRange.length; k++) {
                const auto sectionIndex = formattedString.getSectionIndex(stringIndices[k]);
                const auto& section = formattedString.getSection(stringIndices[k]);
                
                Rect<uint16_t> rect = {
                    static_cast<unsigned short>(CGRectGetMinX(boundingRects[k])),
                    static_cast<unsigned short>(CGRectGetMinY(boundingRects[k])),
                    static_cast<unsigned short>(CGRectGetWidth(boundingRects[k])),
                    static_cast<unsigned short>(CGRectGetHeight(boundingRects[k])),
                };
                
                GlyphMetrics metrics;
                metrics.left = CGRectGetMinX(boundingRects[k]);
                metrics.top = CGRectGetMinY(boundingRects[k]);
                metrics.width = CGRectGetWidth(boundingRects[k]);
                metrics.height = CGRectGetHeight(boundingRects[k]);
                metrics.advance = advances[k].width;
                
                positionedLine.positionedGlyphs.emplace_back(glyphs[k], positions[k].x, positions[k].y, vertical, section.fontStackHash, section.scale, rect, metrics, section.imageID, sectionIndex);
            }
        }
    }

    return shaping;
}

} // namespace mbgl
