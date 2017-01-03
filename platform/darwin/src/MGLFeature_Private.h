#import "MGLFoundation.h"
#import "MGLFeature.h"
#import "MGLShape.h"

#import <mbgl/util/geo.hpp>
#import <mbgl/util/feature.hpp>
#import <mbgl/style/conversion/geojson.hpp>

NS_ASSUME_NONNULL_BEGIN

/**
 Returns an array of `MGLFeature` objects converted from the given vector of
 vector tile features.
 */
MGL_EXPORT
NS_ARRAY_OF(MGLShape <MGLFeature> *) *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::Feature> &features);

/**
 Returns an `MGLFeature` object converted from the given mbgl::Feature
 */
id <MGLFeature> MGLFeatureFromMBGLFeature(const mbgl::Feature &feature);

/**
 Returns an `MGLShape` representing the given geojson. The shape can be
 a feature, a collection of features, or a geometry.
 */
MGLShape* MGLShapeFromGeoJSON(const mapbox::geojson::geojson &geojson);

/**
 Takes an `mbgl::Feature` object, an identifer, and attributes dictionary and
 returns the feature object with converted `mbgl::FeatureIdentifier` and 
 `mbgl::PropertyMap` properties.
 */
mbgl::Feature mbglFeature(mbgl::Feature feature, id identifier, NSDictionary *attributes);

/**
 Returns an `NSDictionary` representation of an `MGLFeature`.
 */
NS_DICTIONARY_OF(NSString *, id) *NSDictionaryFeatureForGeometry(NSDictionary *geometry, NSDictionary *attributes, id identifier);

NS_ASSUME_NONNULL_END
