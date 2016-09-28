#import "MGLTileSet.h"

#include <mbgl/util/tileset.hpp>

@interface MGLTileSet (Private)

- (mbgl::Tileset)mbglTileset;

@end 