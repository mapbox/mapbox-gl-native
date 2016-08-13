#import "MGLTileSet.h"

#include <mbgl/util/tileset.hpp>

NS_ASSUME_NONNULL_BEGIN

@class MGLAttributionInfo;

@interface MGLTileSet (Private)

/**
 A structured representation of the `attribution` property. The default value is
 `nil`.
 */
@property (nonatomic, copy, nullable) NS_ARRAY_OF(MGLAttributionInfo *) *attributionInfos;

- (mbgl::Tileset)mbglTileset;

@end

NS_ASSUME_NONNULL_END
