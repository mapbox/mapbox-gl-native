#import <Foundation/Foundation.h>

#import "MGLPolyline.h"
#import "MGLPolygon.h"
#import "MGLPointAnnotation.h"
#import "MGLShapeCollection.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLFeature` protocol is used to provide details about geographic features
 contained in a map view’s
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile sources</a>.
 Each concrete subclass of `MGLShape` in turn has a subclass that conforms to
 this protocol.
 
 Typically, you do not create feature objects yourself but rather obtain them
 using `-[MGLMapView visibleFeaturesAtPoint:]` and related methods. Each feature
 object associates a shape with an identifier and attributes as specified by the
 source. Like ordinary `MGLAnnotation` objects, some kinds of `MGLFeature`
 objects can also be added to a map view using `-[MGLMapView addAnnotations:]`
 and related methods.
 */
@protocol MGLFeature <MGLAnnotation>

/**
 An object that uniquely identifies the feature in its containing
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile source</a>.
 
 The value of this property is currently always an `NSNumber` object but may in
 the future be an instance of another class, such as `NSString`.
 
 The identifier corresponds to the
 <a href="https://github.com/mapbox/vector-tile-spec/tree/master/2.1#42-features">feature identifier</a>
 (`id`) in the tile source. If the source does not specify the feature’s
 identifier, the value of this property is `nil`.
 
 For details about the identifiers used in most Mapbox-provided styles, consult
 the
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets/">Mapbox Streets</a>
 layer reference.
 */
@property (nonatomic, copy, nullable, readonly) id identifier;

/**
 A dictionary of attributes for this feature specified by the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile source</a>.
 
 The keys and values of this dictionary are determined by the tile source. In
 the tile source, each attribute name is a string, while each attribute value
 may be a null value, Boolean value, integer, floating-point number, or string.
 These data types are mapped to instances of the following Foundation classes:
 
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
 */
@property (nonatomic, copy, readonly) NS_DICTIONARY_OF(NSString *, id) *attributes;

/**
 Returns the feature attribute for the given attribute name.
 
 See the `attributes` property’s documentation for details on keys and values
 associated with this method.
 */
- (nullable id)attributeForKey:(NSString *)key;

@end

/**
 The `MGLPointFeature` class represents a point in a
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile source</a>.
 */
@interface MGLPointFeature : MGLPointAnnotation <MGLFeature>
@end

/**
 The `MGLPolylineFeature` class represents a polyline in a
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile source</a>.
 */
@interface MGLPolylineFeature : MGLPolyline <MGLFeature>
@end

/**
 The `MGLPolygonFeature` class represents a polygon in a
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile source</a>.
 */
@interface MGLPolygonFeature : MGLPolygon <MGLFeature>
@end

/**
 The `MGLMultiPointFeature` class represents a multipoint in a
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile source</a>.
 */
@interface MGLMultiPointFeature : MGLMultiPoint <MGLFeature>
@end

/**
 The `MGLMultiPolylineFeature` class represents a multipolyline in a
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile source</a>.
 */
@interface MGLMultiPolylineFeature : MGLMultiPolyline <MGLFeature>
@end

/**
 The `MGLMultiPolygonFeature` class represents a multipolygon in a
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile source</a>.
 */
@interface MGLMultiPolygonFeature : MGLMultiPolygon <MGLFeature>
@end

/**
 The `MGLShapeCollectionFeature` class represents a shape collection in a
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile source</a>.
 */
@interface MGLShapeCollectionFeature : MGLShapeCollection <MGLFeature>
@end

NS_ASSUME_NONNULL_END
