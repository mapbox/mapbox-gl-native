#import "MGLRasterTileSource.h"

#include <memory>
#include <mbgl/util/variant.hpp>

namespace mbgl {
    class Tileset;
    namespace style {
        class RasterSource;
    }
}

NS_ASSUME_NONNULL_BEGIN

@interface MGLRasterTileSource (Private)

@property (nonatomic, readonly, nullable) mbgl::style::RasterSource *rawSource;

- (std::unique_ptr<mbgl::style::RasterSource>)pendingSourceWithIdentifier:(NSString *)identifier urlOrTileset:(mbgl::variant<std::string, mbgl::Tileset>)urlOrTileset tileSize:(uint16_t)tileSize;

@end

NS_ASSUME_NONNULL_END
