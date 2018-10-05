#import <CoreGraphics/CoreGraphics.h>

#import "MGLFoundation.h"
#import "MGLTileSource.h"

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    class Tileset;
}

@class MGLAttributionInfo;

@interface MGLTileSource (Private)

/**
 An HTML string to be displayed as attribution when the map is shown to a user.

 The default value is `nil`. If the source is initialized with a configuration
 URL, this property is also `nil` until the configuration JSON file is loaded.
 */
@property (nonatomic, copy, nullable, readonly) NSString *attributionHTMLString;

/**
 A structured representation of the `attribution` property. The default value is
 `nil`.

 @param fontSize The default text size in points, or 0 to use the default.
 @param linkColor The default link color, or `nil` to use the default.
 */
- (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosWithFontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor;

@end

MGL_EXPORT
mbgl::Tileset MGLTileSetFromTileURLTemplates(NS_ARRAY_OF(NSString *) *tileURLTemplates, NS_DICTIONARY_OF(MGLTileSourceOption, id) * _Nullable options);

NS_ASSUME_NONNULL_END
