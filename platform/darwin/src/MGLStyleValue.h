#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

NS_ASSUME_NONNULL_BEGIN

/**
 An `MGLStyleValue` object is a generic container for a style attribute value.
 The layout and paint attribute properties of `MGLStyleLayer` can be set to
 `MGLStyleValue` objects.
 
 The `MGLStyleValue` class itself represents a class cluster. Under the hood, a
 particular `MGLStyleValue` object may be either an `MGLStyleConstantValue` to
 represent a constant value or an `MGLStyleFunction` to represent a value
 function. Do not initialize an `MGLStyleValue` object directly; instead, use
 one of the class factory methods to create an `MGLStyleValue` object.
 
 The `MGLStyleValue` class takes a generic parameter `T` that indicates the
 Foundation class being wrapped by this class. Common values for `T` include:

 <ul>
 <li><code>NSNumber</code> (for Boolean values and floating-point numbers)</li>
 <li><code>NSValue</code> (for <code>CGVector</code>, <code>NSEdgeInsets</code>, <code>UIEdgeInsets</code>, and enumerations)</li>
 <li><code>NSString</code></li>
 <li><code>NSColor</code> or <code>UIColor</code></li>
 <li><code>NSArray</code></li>
 </ul>
 */
@interface MGLStyleValue<T> : NSObject

#pragma mark Creating a Style Value

/**
 Creates and returns an `MGLStyleConstantValue` object containing a raw value.
 
 @param rawValue The constant value contained by the object.
 @return An `MGLStyleConstantValue` object containing `rawValue`, which is
    treated as a constant value.
 */
+ (instancetype)valueWithRawValue:(T)rawValue;

/**
 Creates and returns an `MGLStyleFunction` object representing a linear zoom
 level function with any number of stops.
 
 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLStyleFunction` object with the given stops.
 */
+ (instancetype)valueWithStops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

/**
 Creates and returns an `MGLStyleFunction` object representing a zoom level
 function with an exponential interpolation base and any number of stops.
 
 @param interpolationBase The exponential base of the interpolation curve.
 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLStyleFunction` object with the given interpolation base and stops.
 */
+ (instancetype)valueWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

@end

/**
 An `MGLStyleConstantValue` object is a generic container for a style attribute
 value that remains constant as the zoom level changes. The layout and paint
 attribute properties of `MGLStyleLayer` objects can be set to
 `MGLStyleConstantValue` objects.
 
 The `MGLStyleConstantValue` class takes a generic parameter `T` that indicates
 the Foundation class being wrapped by this class.
 */
@interface MGLStyleConstantValue<T> : MGLStyleValue<T>

#pragma mark Creating a Style Constant Value

/**
 Creates and returns an `MGLStyleConstantValue` object containing a raw value.
 
 @param rawValue The constant value contained by the object.
 @return An `MGLStyleConstantValue` object containing `rawValue`, which is
    treated as a constant value.
 */
+ (instancetype)valueWithRawValue:(T)rawValue;

#pragma mark Initializing a Style Constant Value

- (instancetype)init NS_UNAVAILABLE;

/**
 Returns an `MGLStyleConstantValue` object containing a raw value.
 
 @param rawValue The value contained by the receiver.
 @return An `MGLStyleConstantValue` object containing `rawValue`.
 */
- (instancetype)initWithRawValue:(T)rawValue NS_DESIGNATED_INITIALIZER;

#pragma mark Accessing the Underlying Value

/**
 The raw value contained by the receiver.
 */
@property (nonatomic) T rawValue;

@end

/**
 An `MGLStyleFunction` is a value function defining a style value that changes
 as the zoom level changes. The layout and paint attribute properties of an
 `MGLStyleLayer` object can be set to `MGLStyleFunction` objects. Use a zoom
 level function to create the illusion of depth and control data density.
 
 The `MGLStyleFunction` class takes a generic parameter `T` that indicates the
 Foundation class being wrapped by this class.
 */
@interface MGLStyleFunction<T> : MGLStyleValue<T>

#pragma mark Creating a Style Function

/**
 Creates and returns an `MGLStyleFunction` object representing a linear zoom
 level function with any number of stops.
 
 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLStyleFunction` object with the given stops.
 */
+ (instancetype)functionWithStops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

/**
 Creates and returns an `MGLStyleFunction` object representing a zoom level
 function with an exponential interpolation base and any number of stops.
 
 @param interpolationBase The exponential base of the interpolation curve.
 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLStyleFunction` object with the given interpolation base and stops.
 */
+ (instancetype)functionWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

#pragma mark Initializing a Style Function

/**
 Returns an `MGLStyleFunction` object representing a zoom level function with an
 exponential interpolation base and any number of stops.
 
 @param interpolationBase The exponential base of the interpolation curve.
 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLStyleFunction` object with the given interpolation base and stops.
 */
- (instancetype)initWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops NS_DESIGNATED_INITIALIZER;

#pragma mark Accessing the Parameters of a Function

/**
 The exponential interpolation base of the function’s interpolation curve.
 
 The exponential interpolation base controls the rate at which the function’s output values
 increase. A value of 1 causes the function to increase linearly by zoom level.
 A higher exponential interpolation base causes the function’s output values to vary
 exponentially, increasing more rapidly towards the high end of the function’s
 range. The default value of this property is 1, for a linear curve.
 */
@property (nonatomic) CGFloat interpolationBase;

/**
 A dictionary associating zoom levels with style values.
 
 Each of the function’s stops is represented by one key-value pair in the
 dictionary. Each key in the dictionary is an `NSNumber` object containing a
 floating-point zoom level. Each value in the dictionary is an `MGLStyleValue`
 object containing the value of the style attribute when the map is at the
 associated zoom level. An `MGLStyleFunction` object may not be used recursively
 as a stop value.
 */
@property (nonatomic, copy) NSDictionary<NSNumber *, MGLStyleValue<T> *> *stops;

@end

NS_ASSUME_NONNULL_END
