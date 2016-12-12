#import <Foundation/Foundation.h>

#import "MGLAnnotation.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLShape` class is an abstract class that defines the basic properties for
 all shape-based annotation objects. This class must be subclassed and cannot be
 used as is. Subclasses are responsible for defining the geometry of the shape
 and providing an appropriate value for the coordinate property inherited from
 the `MGLAnnotation` protocol.
 */
@interface MGLShape : NSObject <MGLAnnotation>

#pragma mark Creating a Shape

/**
 Returns an `MGLShape` object initialized with the given data interpreted as a
 string containing a GeoJSON object.
 
 If the GeoJSON object is a geometry, the returned value is a kind of
 `MGLShape`. If it is a feature object, the returned value is a kind of
 `MGLShape` that conforms to the `MGLFeature` protocol. If it is a feature
 collection object, the returned value is an instance of
 `MGLShapeCollectionFeature`.
 
 @param data String data containing GeoJSON source code.
 @param encoding The encoding used by `data`.
 @param outError Upon return, if an error has occurred, a pointer to an
    `NSError` object describing the error. Pass in `NULL` to ignore any error.
 @return An `MGLShape` object representation of `data`, or `nil` if `data` could
    not be parsed as valid GeoJSON source code. If `nil`, `outError` contains an
    `NSError` object describing the problem.
 */
+ (nullable instancetype)shapeWithData:(NSData *)data encoding:(NSStringEncoding)encoding error:(NSError * _Nullable *)outError;

#pragma mark Accessing the Shape Attributes

/**
 The title of the shape annotation. The default value of this property is `nil`.
 */
@property (nonatomic, copy, nullable) NSString *title;

/**
 The subtitle of the shape annotation. The default value of this property is
 `nil`.
 */
@property (nonatomic, copy, nullable) NSString *subtitle;

#if !TARGET_OS_IPHONE

/**
 The tooltip of the shape annotation. The default value of this property is
 `nil`.
 */
@property (nonatomic, copy, nullable) NSString *toolTip;

#endif

#pragma mark Creating GeoJSON Data

/**
 Returns the GeoJSON string representation of the shape encapsulated in a data
 object.
 
 @param encoding The string encoding to use.
 @return A data object containing the shape’s GeoJSON string representation.
 */
- (NSData *)geoJSONDataUsingEncoding:(NSStringEncoding)encoding;

@end

NS_ASSUME_NONNULL_END
