#import "MGLRasterSource.h"

namespace mbgl {
    namespace style {
        class RasterSource;
    }
}

@interface MGLRasterSource (Private)

- (instancetype)initWithRawSource:(mbgl::style::RasterSource *)rawSource;

@end
