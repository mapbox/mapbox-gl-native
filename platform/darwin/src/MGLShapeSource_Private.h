#import "MGLFoundation.h"
#import "MGLShapeSource.h"

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    namespace style {
        struct GeoJSONOptions;
    }
}

MGL_EXPORT
mbgl::style::GeoJSONOptions MGLGeoJSONOptionsFromDictionary(NSDictionary<MGLShapeSourceOption, id> *options);

@interface MGLShapeSource (Private)
- (void)debugRecursiveLogForFeature:(id <MGLFeature>)feature indent:(NSUInteger)indent;
@end

NS_ASSUME_NONNULL_END
