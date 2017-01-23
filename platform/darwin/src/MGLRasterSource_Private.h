#import "MGLRasterSource.h"

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    namespace style {
        class RasterSource;
    }
}

@interface MGLRasterSource (Private)

- (instancetype)initWithRawSource:(mbgl::style::RasterSource *)rawSource;

@end

NS_ASSUME_NONNULL_END
