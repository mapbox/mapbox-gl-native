#import "MGLFoundation.h"
#import "MGLShapeSource.h"

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    namespace style {
        class GeoJSONOptions;
        struct GeoJSONSource;
    }
}

@interface MGLShapeSource (Private)

- (instancetype)initWithRawSource:(mbgl::style::GeoJSONSource *)rawSource;

@end

NS_ASSUME_NONNULL_END
