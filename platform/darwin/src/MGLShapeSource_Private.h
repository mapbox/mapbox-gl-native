#import "MGLFoundation.h"
#import "MGLShapeSource.h"

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    namespace style {
        class GeoJSONOptions;
    }
}

@interface MGLShapeSource (Private)
@end

MGL_EXPORT
mbgl::style::GeoJSONOptions MGLGeoJSONOptionsFromDictionary(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *options);

NS_ASSUME_NONNULL_END
