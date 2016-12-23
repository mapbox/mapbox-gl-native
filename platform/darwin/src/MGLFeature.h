#import <Foundation/Foundation.h>

#import "MGLPolyline.h"
#import "MGLPolygon.h"
#import "MGLPointAnnotation.h"
#import "MGLPointCollection.h"
#import "MGLShapeCollection.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLFeature` protocol is used to provide details about geographic features
 contained in an `MGLShapeSource` or `MGLVectorSource` object. Each concrete
 subclass of `MGLShape` in turn has a subclass that conforms to this protocol. A
 feature object associates a shape with an optional identifier and attributes.
 
 You can add custom data to display on the map by creating feature objects and
 adding them to an `MGLShapeSource` using the
 `-[MGLShapeSource initWithIdentifier:shape:options:]` method or
 `MGLShapeSource.shape` property. Similarly, you can add `MGLPointFeature`,
 `MGLPolylineFeature`, and `MGLPolygonFeature` objects to the map as annotations
 using `-[MGLMapView addAnnotations:]` and related methods.
 
 In addition to adding data to the map, you can also extract data from the map:
 `-[MGLMapView visibleFeaturesAtPoint:]` and related methods return feature
 objects that correspond to features in the source. This enables you to inspect
 the properties of features in vector tiles loaded by `MGLVectorSource` objects.
 You also reuse these feature objects as overlay annotations.
 */
@protocol MGLFeature <MGLAnnotation>

/**
 An object that uniquely identifies the feature in its containing content
 source.
 
 You can configure an `MGLVectorStyleLayer` object to include or exclude a
 specific feature in an `MGLShapeSource` or `MGLVectorSource`. In the
 `MGLVectorStyleLayer.predicate` property, compare the special `$id` attribute
 to the feature’s identifier.
 
 In vector tiles loaded by `MGLVectorSource` objects, the identifier corresponds
 to the
 <a href="https://github.com/mapbox/vector-tile-spec/tree/master/2.1#42-features">feature identifier</a>
 (`id`). If the source does not specify the feature’s identifier, the value of
 this property is `nil`. If specified, the identifier may be an integer,
 floating-point number, or string. These data types are mapped to instances of
 the following Foundation classes:
 
 <table>
 <thead>
 <tr><th>In the tile source</th><th>This property</th></tr>
 </thead>
 <tbody>
 <tr><td>Integer</td>               <td><code>NSNumber</code> (use the <code>unsignedLongLongValue</code> or <code>longLongValue</code> property)</td></tr>
 <tr><td>Floating-point number</td> <td><code>NSNumber</code> (use the <code>doubleValue</code> property)</td></tr>
 <tr><td>String</td>                <td><code>NSString</code></td></tr>
 </tbody>
 </table>
 
 For details about the identifiers used in most Mapbox-provided styles, consult
 the
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets/">Mapbox Streets</a>
 layer reference.
 
 The identifier should be set before adding the feature to an `MGLShapeSource`
 object; setting it afterwards has no effect on the map’s contents. While it is
 possible to change this value on feature instances obtained from
 `-[MGLMapView visibleFeaturesAtPoint:]` and related methods, doing so likewise
 has no effect on the map’s contents.
 */
@property (nonatomic, copy, nullable) id identifier;

/**
 A dictionary of attributes for this feature.
 
 You can configure an `MGLVectorStyleLayer` object to include or exclude a
 specific feature in an `MGLShapeSource` or `MGLVectorSource`. In the
 `MGLVectorStyleLayer.predicate` property, compare a key of the attribute
 dictionary to the value you want to include. For example, if you want an
 `MGLLineStyleLayer` object to display only important features, you might assign
 a value above 50 to the important features’ `importance` attribute, then set
 `MGLVectorStyleLayer.predicate` to an `NSPredicate` with the format
 `importance > 50`.
 
 You can also configure some attributes of an `MGLSymbolStyleLayer` object to
 include the value of an attribute in this dictionary whenever it renders this
 feature. For example, to label features in an `MGLShapeSource` object by their
 names, you can assign a `name` attribute to each of the source’s features, then
 set `MGLSymbolStyleLayer.textField` to an `MGLStyleValue` object containing the
 string `{name}`.
 
 In vector tiles loaded by `MGLVectorSource` objects, the keys and values of
 each feature’s attribute dictionary are determined by the source. Each
 attribute name is a string, while each attribute value may be a null value,
 Boolean value, integer, floating-point number, or string. These data types are
 mapped to instances of the following Foundation classes:
 
 <table>
 <thead>
 <tr><th>In the tile source</th><th>In this dictionary</th></tr>
 </thead>
 <tbody>
 <tr><td>Null</td>                  <td><code>NSNull</code></td></tr>
 <tr><td>Boolean</td>               <td><code>NSNumber</code> (use the <code>boolValue</code> property)</td></tr>
 <tr><td>Integer</td>               <td><code>NSNumber</code> (use the <code>unsignedLongLongValue</code> or <code>longLongValue</code> property)</td></tr>
 <tr><td>Floating-point number</td> <td><code>NSNumber</code> (use the <code>doubleValue</code> property)</td></tr>
 <tr><td>String</td>                <td><code>NSString</code></td></tr>
 </tbody>
 </table>
 
 For details about the attribute names and values found in Mapbox-provided
 vector tile sources, consult the
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets/">Mapbox Streets</a>
 and
 <a href="https://www.mapbox.com/vector-tiles/mapbox-terrain/">Mapbox Terrain</a>
 layer references.
 
 Note that while it is possible to change this value on feature
 instances obtained from `-[MGLMapView visibleFeaturesAtPoint:]` and related
 methods, there will be no effect on the map. Setting this value can be useful
 when the feature instance is used to initialize an `MGLShapeSource` and that
 source is added to the map and styled.
 */
@property (nonatomic, copy) NS_DICTIONARY_OF(NSString *, id) *attributes;

/**
 Returns the feature attribute for the given attribute name.
 
 See the `attributes` property’s documentation for details on keys and values
 associated with this method.
 */
- (nullable id)attributeForKey:(NSString *)key;

/**
 Returns a dictionary that can be serialized as a GeoJSON Feature representation
 of an instance of an `MGLFeature` subclass.
 
 The dictionary includes a `geometry` key corresponding to the receiver’s 
 underlying geometry data, a `properties` key corresponding to the receiver’s 
 `attributes` property, and an `id` key corresponding to the receiver’s 
 `identifier` property.
 */
- (NS_DICTIONARY_OF(NSString *, id) *)geoJSONDictionary;

@end

/**
 An `MGLPointFeature` object associates a point shape with an optional
 identifier and attributes.
 */
@interface MGLPointFeature : MGLPointAnnotation <MGLFeature>
@end

/**
 An `MGLPolylineFeature` object associates a polyline shape with an optional
 identifier and attributes.
 
 A polyline feature is known as a
 <a href="https://tools.ietf.org/html/rfc7946#section-3.1.4">LineString</a>
 feature in GeoJSON.
 */
@interface MGLPolylineFeature : MGLPolyline <MGLFeature>
@end

/**
 An `MGLPolygonFeature` object associates a polygon shape with an optional
 identifier and attributes.
 */
@interface MGLPolygonFeature : MGLPolygon <MGLFeature>
@end

/**
 An `MGLPointCollectionFeature` object associates a point collection with an
 optional identifier and attributes.
 
 A point collection feature is known as a
 <a href="https://tools.ietf.org/html/rfc7946#section-3.1.3">MultiPoint</a>
 feature in GeoJSON.
 */
@interface MGLPointCollectionFeature : MGLPointCollection <MGLFeature>
@end

// https://github.com/mapbox/mapbox-gl-native/issues/7473
@compatibility_alias MGLMultiPointFeature MGLPointCollectionFeature;

/**
 An `MGLMultiPolylineFeature` object associates a multipolyline shape with an
 optional identifier and attributes.
 
 A multipolyline feature is known as a
 <a href="https://tools.ietf.org/html/rfc7946#section-3.1.5">MultiLineString</a>
 feature in GeoJSON.
 */
@interface MGLMultiPolylineFeature : MGLMultiPolyline <MGLFeature>
@end

/**
 An `MGLMultiPolygonFeature` object associates a multipolygon shape with an
 optional identifier and attributes.
 */
@interface MGLMultiPolygonFeature : MGLMultiPolygon <MGLFeature>
@end

/**
 An `MGLShapeCollectionFeature` object associates a shape collection with an
 optional identifier and attributes.
 
 A shape collection feature is known as a
 <a href="https://tools.ietf.org/html/rfc7946#section-3.3">feature collection</a>
 in GeoJSON.
 */
@interface MGLShapeCollectionFeature : MGLShapeCollection <MGLFeature>

@property (nonatomic, copy, readonly) NS_ARRAY_OF(MGLShape<MGLFeature> *) *shapes;

+ (instancetype)shapeCollectionWithShapes:(NS_ARRAY_OF(MGLShape<MGLFeature> *) *)shapes;

@end

NS_ASSUME_NONNULL_END
