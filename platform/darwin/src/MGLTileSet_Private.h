#import "MGLTileSet.h"

#include <mbgl/util/tileset.hpp>

NS_ASSUME_NONNULL_BEGIN

@class MGLAttributionInfo;

@interface MGLTileSet (Private)

/**
 A structured representation of the `attribution` property. The default value is
 `nil`.
 
 @param fontSize The default text size in points.
 @param linkColor The default link color.
 */
- (nullable NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosWithFontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor;

- (mbgl::Tileset)mbglTileset;

@end

NS_ASSUME_NONNULL_END
