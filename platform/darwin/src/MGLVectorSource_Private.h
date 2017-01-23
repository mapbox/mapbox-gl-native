#import "MGLVectorSource.h"

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    namespace style {
        class VectorSource;
    }
}

@interface MGLVectorSource (Private)

- (instancetype)initWithRawSource:(mbgl::style::VectorSource *)rawSource;

@end

NS_ASSUME_NONNULL_END
