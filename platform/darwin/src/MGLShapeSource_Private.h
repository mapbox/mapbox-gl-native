#import "MGLShapeSource.h"
#import "MGLShapeSource_Private.h"

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

mbgl::style::GeoJSONOptions MGLGeoJSONOptionsFromDictionary(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *options);

NS_ASSUME_NONNULL_END
