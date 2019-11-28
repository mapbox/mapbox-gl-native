#import "MGLFoundation.h"
#import "MGLShapeSource.h"

#include <mbgl/util/immutable.hpp>

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    namespace style {
        struct GeoJSONOptions;
    }
}

MGL_EXPORT
mbgl::Immutable<mbgl::style::GeoJSONOptions> MGLGeoJSONOptionsFromDictionary(NSDictionary<MGLShapeSourceOption, id> *options);

@interface MGLShapeSource (Private)

/**
 :nodoc:
 Debug log showing structure of an `MGLFeature`. This method recurses in the case
 that the feature conforms to `MGLCluster`. This method is used for testing and
 should be considered experimental, likely to be removed or changed in future
 releases.
 
 @param feature An object that conforms to the `MGLFeature` protocol.
 @param indent Used during recursion. Specify 0.
 */

- (void)debugRecursiveLogForFeature:(id<MGLFeature>)feature indent:(NSUInteger)indent;
@end

NS_ASSUME_NONNULL_END
