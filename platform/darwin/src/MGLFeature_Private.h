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
NSArray<MGLShape <MGLFeature> *> *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::Feature> &features);

/**
 Returns an array of `MGLFeature` objects converted from the given vector of
 vector tile features.
 */
MGL_EXPORT
NSArray<MGLShape <MGLFeature> *> *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::GeoJSONFeature> &features);

/**
 Returns an `MGLFeature` object converted from the given mbgl::GeoJSONFeature
 */
MGL_EXPORT
id <MGLFeature> MGLFeatureFromMBGLFeature(const mbgl::GeoJSONFeature &feature);

/**
 Returns an `MGLShape` representing the given geojson. The shape can be
 a feature, a collection of features, or a geometry.
 */
MGLShape* MGLShapeFromGeoJSON(const mapbox::geojson::geojson &geojson);

/**
 Takes an `mbgl::GeoJSONFeature` object, an identifer, and attributes dictionary and
 returns the feature object with converted `mbgl::FeatureIdentifier` and
 `mbgl::PropertyMap` properties.
 */
mbgl::GeoJSONFeature mbglFeature(mbgl::GeoJSONFeature feature, id identifier, NSDictionary * attributes);

/**
 Returns an `NSDictionary` representation of an `MGLFeature`.
 */
NSDictionary<NSString *, id> *NSDictionaryFeatureForGeometry(NSDictionary *geometry, NSDictionary *attributes, id identifier);

NS_ASSUME_NONNULL_END

#define MGL_DEFINE_FEATURE_INIT_WITH_CODER() \
    - (instancetype)initWithCoder:(NSCoder *)decoder { \
        if (self = [super initWithCoder:decoder]) { \
            NSSet<Class> *identifierClasses = [NSSet setWithArray:@[[NSString class], [NSNumber class]]]; \
            identifier = [decoder decodeObjectOfClasses:identifierClasses forKey:@"identifier"]; \
            _attributes = [decoder decodeObjectOfClass:[NSDictionary class] forKey:@"attributes"]; \
        } \
        return self; \
    }

#define MGL_DEFINE_FEATURE_ENCODE() \
    - (void)encodeWithCoder:(NSCoder *)coder { \
        [super encodeWithCoder:coder]; \
        [coder encodeObject:identifier forKey:@"identifier"]; \
        [coder encodeObject:_attributes forKey:@"attributes"]; \
    }

#define MGL_DEFINE_FEATURE_IS_EQUAL() \
    - (BOOL)isEqual:(id)other { \
        if (other == self) return YES; \
        if (![other isKindOfClass:[self class]]) return NO; \
        __typeof(self) otherFeature = other; \
        return [super isEqual:other] && [self geoJSONObject] == [otherFeature geoJSONObject]; \
    } \
    - (NSUInteger)hash { \
        return [super hash] + [[self geoJSONDictionary] hash]; \
    }

#define MGL_DEFINE_FEATURE_ATTRIBUTES_GETTER() \
    - (NSDictionary *) attributes { \
        if (!_attributes) { \
            return @{}; \
        } \
        return _attributes; \
    }
