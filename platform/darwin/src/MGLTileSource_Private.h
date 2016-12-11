#import "MGLTileSource.h"

#import <CoreGraphics/CoreGraphics.h>

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    class Tileset;
}

@class MGLAttributionInfo;

@interface MGLTileSource (Private)

/**
 A structured representation of the `attribution` property. The default value is
 `nil`.
 
 @param fontSize The default text size in points.
 @param linkColor The default link color.
 */
- (nullable NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosWithFontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor;

@end

mbgl::Tileset MGLTileSetFromTileURLTemplates(NS_ARRAY_OF(NSString *) *tileURLTemplates, NS_DICTIONARY_OF(MGLTileSourceOption, id) * _Nullable options);

NS_ASSUME_NONNULL_END
