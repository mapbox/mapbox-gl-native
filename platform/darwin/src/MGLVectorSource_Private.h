#import "MGLVectorSource.h"

namespace mbgl {
    namespace style {
        class VectorSource;
    }
}

@interface MGLVectorSource (Private)

- (instancetype)initWithRawSource:(mbgl::style::VectorSource *)rawSource;

@end
