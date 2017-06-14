#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

#import "MGLFoundation.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Options for `MGLStyleFunction` objects.
 */
typedef NSString *MGLStyleFunctionOption NS_STRING_ENUM;

/**
 An `NSNumber` object containing an integer that determines the style function's 
 exponential interpolation base.
 
 The exponential interpolation base controls the rate at which the function’s 
 output values increase. A value of 1 causes the function to increase linearly 
 based on zoom level or attribute value. A higher exponential interpolation base
 causes the function’s output values to vary exponentially, increasing more rapidly
 towards the high end of the function’s range. The default value of this property 
 is 1, for a linear curve.

 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#function-base"><code>base</code></a>
 function property in the Mapbox Style Specification.

 This option only applies to functions that use an 
 `MGLInterpolationModeExponential` interpolation mode that are assigned to
 interpolatable style layer properties.
 */
extern MGL_EXPORT const MGLStyleFunctionOption MGLStyleFunctionOptionInterpolationBase;

/**
 An `MGLConstantStyleValue` object that specifies a default value that a style
 function can use when it can't otherwise determine a value.

 A default value can be used to set the value of a style layer property that
 is not otherwise set by a function. For example, a source function with a
 `MGLInterpolationModeCategorical` interpolation mode with stops that specify
 color values to use based on a feature's attributes would set any feature
 that does not have attributes that match the stop key values to this 
 default value.
 
 This option only applies to `MGLSourceStyleFunction` and
 `MGLCompositeStyleFunction` functions.
 */
extern MGL_EXPORT const MGLStyleFunctionOption MGLStyleFunctionOptionDefaultValue;

/** 
 The modes used to interpolate property values between map zoom level changes
 or over a range of feature attribute values.
 */
typedef NS_ENUM(NSUInteger, MGLInterpolationMode) {
    /**
     Values between two stops are interpolated linearly, or exponentially based on 
     the `MGLStyleFunctionOptionInterpolationBase`. A higher interpolation base 
     causes the function’s output values to vary exponentially, increasing more rapidly
     towards the high end of the function’s range. The default interpolation base of 1 
     creates a linear interpolation. Use exponential interpolation mode to show values
     relative to stop keys.
     */
    MGLInterpolationModeExponential = 0,
    /**
     Values between two stops are not interpolated. Instead, properties are set
     to the value of the stop just less than the function input. Use interval
     interpolation mode to show values that fall within a range.
     */
    MGLInterpolationModeInterval,
    /**
     Values between two stops are not interpolated. Instead, properties are set
     to the value of the stop equal to the function input's key value. Use
     categorical interpolation mode to show values that fit into categories.
     */
    MGLInterpolationModeCategorical,
    /**
     Values between two stops are not interpolated. Instead, for any given feature, the
     style value matches a value in that feature’s attributes dictionary. Use identity
     interpolation mode to show attribute values that can be used as style values.
     */
    MGLInterpolationModeIdentity
};

/**
 An `MGLStyleValue` object is a generic container for a style attribute value.
 The layout and paint attribute properties of `MGLStyleLayer` can be set to
 `MGLStyleValue` objects.

 The `MGLStyleValue` class itself represents a class cluster. Under the hood, a
 particular `MGLStyleValue` object may be either an `MGLConstantStyleValue` to
 represent a constant value or one of the concrete subclasses of 
 `MGLStyleFunction` to represent a value function. Do not initialize an 
 `MGLStyleValue` object directly; instead, use one of the class factory methods 
 to create an `MGLStyleValue` object.

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
MGL_EXPORT
@interface MGLStyleValue<T> : NSObject

#pragma mark Creating a Style Value

/**
 Creates and returns an `MGLConstantStyleValue` object containing a raw value.

 @param rawValue The constant value contained by the object.
 @return An `MGLConstantStyleValue` object containing `rawValue`, which is
    treated as a constant value.
 */
+ (instancetype)valueWithRawValue:(T)rawValue;

#pragma mark Function values

/**
 Creates and returns an `MGLCameraStyleFunction` object representing a linear camera
 function with one or more stops.

 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLCameraStyleFunction` object with the given stops.
 */
+ (instancetype)valueWithStops:(NS_DICTIONARY_OF(NSNumber *, MGLStyleValue<T> *) *)stops __attribute__((deprecated("Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:]")));

/**
 Creates and returns an `MGLCameraStyleFunction` object representing a camera
 function with an exponential interpolation base and one or more stops.

 @param interpolationBase The exponential base of the interpolation curve.
 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLCameraStyleFunction` object with the given interpolation base and stops.
 */
+ (instancetype)valueWithInterpolationBase:(CGFloat)interpolationBase stops:(NS_DICTIONARY_OF(NSNumber *, MGLStyleValue<T> *) *)stops __attribute__((deprecated("Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:]")));

/**
 Creates and returns an `MGLCameraStyleFunction` object representing a camera function
 with one or more stops.

 @param interpolationMode The mode used to interpolate property values between 
    map zoom level changes.
 @param cameraStops A dictionary associating zoom levels with style values.
 @param options A dictionary containing `MGLStyleFunctionOption` values that 
    specify how a function is applied.
 @return An `MGLCameraStyleFunction` object with the given interpolation mode,
    camera stops, and options.
 */
+ (instancetype)valueWithInterpolationMode:(MGLInterpolationMode)interpolationMode cameraStops:(NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)cameraStops options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

/**
 Creates and returns an `MGLSourceStyleFunction` object representing a source function.

 @param interpolationMode The mode used to interpolate property values over a 
    range of feature attribute values.
 @param sourceStops A dictionary associating feature attributes with style values.
 @param attributeName Specifies the feature attribute to take as the function 
    input.
 @param options A dictionary containing `MGLStyleFunctionOption` values that
    specify how a function is applied.
 @return An `MGLSourceStyleFunction` object with the given interpolation mode,
    source stops, attribute name, and options.
 */
+ (instancetype)valueWithInterpolationMode:(MGLInterpolationMode)interpolationMode sourceStops:(nullable NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)sourceStops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

/**
 Creates and returns an `MGLCompositeStyleFunction` object representing a composite
 function.

 @param interpolationMode The mode used to interpolate property values over a 
    range of feature attribute values for each outer zoom level.
 @param compositeStops A dictionary associating feature attributes with style
    values.
 @param attributeName Specifies the feature attribute to take as the function
    input.
 @param options A dictionary containing `MGLStyleFunctionOption` values that
    specify how a function is applied.
 @return An `MGLCompositeStyleFunction` object with the given interpolation mode,
    composite stops, attribute name, and options.
 */
+ (instancetype)valueWithInterpolationMode:(MGLInterpolationMode)interpolationMode compositeStops:(NS_DICTIONARY_OF(id, NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *) *)compositeStops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

@end

/**
 An `MGLConstantStyleValue` object is a generic container for a style attribute
 value that remains constant as the zoom level changes. The layout and paint
 attribute properties of `MGLStyleLayer` objects can be set to
 `MGLConstantStyleValue` objects.

 The `MGLConstantStyleValue` class takes a generic parameter `T` that indicates
 the Foundation class being wrapped by this class.
 */
MGL_EXPORT
@interface MGLConstantStyleValue<T> : MGLStyleValue<T>

#pragma mark Creating a Style Constant Value

/**
 Creates and returns an `MGLConstantStyleValue` object containing a raw value.

 @param rawValue The constant value contained by the object.
 @return An `MGLConstantStyleValue` object containing `rawValue`, which is
    treated as a constant value.
 */
+ (instancetype)valueWithRawValue:(T)rawValue;

#pragma mark Initializing a Style Constant Value

- (instancetype)init NS_UNAVAILABLE;

/**
 Returns an `MGLConstantStyleValue` object containing a raw value.

 @param rawValue The value contained by the receiver.
 @return An `MGLConstantStyleValue` object containing `rawValue`.
 */
- (instancetype)initWithRawValue:(T)rawValue NS_DESIGNATED_INITIALIZER;

#pragma mark Accessing the Underlying Value

/**
 The raw value contained by the receiver.
 */
@property (nonatomic) T rawValue;

@end

@compatibility_alias MGLStyleConstantValue MGLConstantStyleValue;

/**
 An `MGLStyleFunction` is a is an abstract superclass for functions that are 
 defined by an `MGLCameraStyleFunction`, `MGLSourceStyleFunction`, or
 `MGLCompositeStyleFunction` object.
 
 Create instances of `MGLCameraStyleFunction`, `MGLSourceStyleFunction`, and
 `MGLCompositeStyleFunction` in order to use `MGLStyleFunction`'s methods. Do
 not create instances of `MGLStyleFunction` directly, and do not create your
 own subclasses of this class.

 The `MGLStyleFunction` class takes a generic parameter `T` that indicates the
 Foundation class being wrapped by this class.
 */
MGL_EXPORT
@interface MGLStyleFunction<T> : MGLStyleValue<T>

#pragma mark Creating a Style Function

/**
 Creates and returns an `MGLCameraStyleFunction` object representing a camera 
 function with a linear interpolation curve.

 @note Do not create function instances using this method unless it is required 
    for backwards compatiblity with your application code.

 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLCameraStyleFunction` object with the given stops.
 */
+ (instancetype)functionWithStops:(NS_DICTIONARY_OF(NSNumber *, MGLStyleValue<T> *) *)stops __attribute__((deprecated("Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:]")));

/**
 Creates and returns an `MGLCameraStyleFunction` object representing a camera
 function with an interpolation curve controlled by the provided interpolation 
 base.

 @note Do not create function instances using this method unless it is required
    for backwards compatiblity with your application code.

 @param interpolationBase The exponential base of the interpolation curve.
 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLCameraStyleFunction` object with the given interpolation base and stops.
 */
+ (instancetype)functionWithInterpolationBase:(CGFloat)interpolationBase stops:(NS_DICTIONARY_OF(NSNumber *, MGLStyleValue<T> *) *)stops __attribute__((deprecated("Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:]")));

#pragma mark Initializing a Style Function

/**
 Returns an `MGLStyleFunction` object representing a camera function. If the 
 function is set as a style layer property value, it will be interpreted
 as a camera function with an interpolation curve controlled by the provided
 interpolation base.
 
 @note Do not create instances of `MGLStyleFunction` unless it is required for
    backwards compatiblity with your application code. You should create and use
    instances of `MGLCameraStyleFunction` to specify how properties will
    be visualized at different zoom levels.

 @param interpolationBase The exponential base of the interpolation curve.
 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLStyleFunction` object with the given interpolation base and stops.
 */
- (instancetype)initWithInterpolationBase:(CGFloat)interpolationBase stops:(NS_DICTIONARY_OF(NSNumber *, MGLStyleValue<T> *) *)stops __attribute__((deprecated("Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:]")));

#pragma mark Accessing the Parameters of a Function

/**
 The modes used to interpolate property values between map zoom level changes or
 over a range of feature attribute values.
 */
@property (nonatomic) MGLInterpolationMode interpolationMode;

/**
 A dictionary associating zoom levels with style values.
 */
@property (nonatomic, copy, nullable) NSDictionary *stops;

/**
 The exponential interpolation base of the function’s interpolation curve.
 
 @note This property specifies the exponential base of the interpolation curve 
    of `MGLCameraStyleFunction` and `MGLSourceStyleFunction` functions that use 
    a `MGLInterpolationModeExponential` `interpolationMode`. Otherwise, it is
    ignored.
 */
@property (nonatomic) CGFloat interpolationBase;

@end

/**
 An `MGLCameraStyleFunction` is a value function defining a style value that changes
 as the zoom level changes. The layout and paint attribute properties of an
 `MGLStyleLayer` object can be set to `MGLCameraStyleFunction` objects. Use a camera
 function to create the illusion of depth and control data density.

 The `MGLCameraStyleFunction` class takes a generic parameter `T` that indicates the
 Foundation class being wrapped by this class.
 */
MGL_EXPORT
@interface MGLCameraStyleFunction<T> : MGLStyleFunction<T>

#pragma mark Creating a Camera Function

/**
 Creates and returns an `MGLCameraStyleFunction` object representing a camera
 function with one or more stops.

 @param interpolationMode The mode used to interpolate property values between
    map zoom level changes.
 @param stops A dictionary associating zoom levels with style values.
 @param options A dictionary containing `MGLStyleFunctionOption` values that
    specify how a function is applied.
 @return An `MGLCameraStyleFunction` object with the given interpolation mode,
    camera stops, and options.
 */
+ (instancetype)functionWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)stops options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

#pragma mark Accessing the Parameters of a Camera Function

/**
 A dictionary associating zoom levels with style values.

 Each of the function’s stops is represented by one key-value pair in the
 dictionary. Each key in the dictionary is an `NSNumber` object containing a
 floating-point zoom level. Each value in the dictionary is an `MGLStyleValue`
 object containing the value of the style attribute when the map is at the
 associated zoom level. An `MGLStyleFunction` object may not be used recursively
 as a stop value.
 */
@property (nonatomic, copy) NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *stops;

@end

/**
 An `MGLSourceStyleFunction` is a value function defining a style value that 
 changes with its properties. The layout and paint attribute properties of an
 `MGLStyleLayer` object can be set to `MGLSourceStyleFunction` objects.
 Use source functions to visually differentate types of features within the same 
 layer or create data visualizations.

 The `MGLSourceStyleFunction` class takes a generic parameter `T` that indicates the
 Foundation class being wrapped by this class.
 */
MGL_EXPORT
@interface MGLSourceStyleFunction<T> : MGLStyleFunction<T>

#pragma mark Creating a Source Function

/**
 Creates and returns an `MGLSourceStyleFunction` object representing a source 
 function.

 @param interpolationMode The mode used to interpolate property values over a
    range of feature attribute values.
 @param stops A dictionary associating feature attributes with style values.
 @param attributeName Specifies the feature attribute to take as the function
    input.
 @param options A dictionary containing `MGLStyleFunctionOption` values that
    specify how a function is applied.
 @return An `MGLSourceStyleFunction` object with the given interpolation mode,
    source stops, attribute name, and options.
*/
+ (instancetype)functionWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(nullable NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)stops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

#pragma mark Accessing the Parameters of a Source Function

/**
 A string that specifies the feature attribute key whose value be used as the function
 input.
*/
@property (nonatomic, copy) NSString *attributeName;

/**
 A dictionary associating attribute values with style values.

 Each of the function’s stops is represented by one key-value pair in the
 dictionary. Each key in the dictionary is an object representing a feature 
 attribute key or interpolation stop. Each value in the dictionary is an 
 `MGLStyleValue` object containing the value to use when the function is given 
 the associated attribute key. An `MGLStyleFunction` object may not be used 
 recursively as a stop value.
 */
@property (nonatomic, copy, nullable) NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *stops;

/**
 An `MGLStyleValue` object containing the default value to use when there is
 no input to provide to the function.
 */
@property (nonatomic, nullable) MGLStyleValue<T> *defaultValue;

@end

/**
 An `MGLCompositeStyleFunction` is a value function defining a style value that
 changes with the feature attributes at each map zoom level. The layout and paint 
 attribute properties of an `MGLStyleLayer` object can be set to 
 `MGLCompositeStyleFunction` objects. Use composite functions to allow the 
 appearance of a map feature to change with both its attributes and the map zoom 
 level.

 The `MGLCompositeStyleFunction` class takes a generic parameter `T` that indicates the
 Foundation class being wrapped by this class.
 */
MGL_EXPORT
@interface MGLCompositeStyleFunction<T> : MGLStyleFunction<T>

#pragma mark Creating a Composite Function

/**
 Creates and returns an `MGLCompositeStyleFunction` object representing a composite
 function.

 @param interpolationMode The mode used to interpolate property values over a
    range of feature attribute values for each outer zoom level.
 @param stops A dictionary associating feature attributes with style values.
 @param attributeName Specifies the feature attribute to take as the function
    input.
 @param options A dictionary containing `MGLStyleFunctionOption` values that
    specify how a function is applied.
 @return An `MGLCompositeStyleFunction` object with the given interpolation mode,
    composite stops, attribute name, and options.
 */
+ (instancetype)functionWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NS_DICTIONARY_OF(id, NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *) *)stops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

#pragma mark Accessing the Parameters of a Composite Function

/**
 A string that specifies the feature attribute key whose value be used as the function
 input.
 */
@property (nonatomic, copy) NSString *attributeName;

/**
 A dictionary associating attribute values with style values.

 Each of the function’s stops is represented by one key-value pair in the
 dictionary. Each key in the dictionary is an `NSNumber` object containing a
 floating-point zoom level. Each value in the dictionary is an inner nested 
 dictionary. Each key in the nested dictionary is an object representing a feature
 attribute key or interpolation stop. Each value in the nested dictionary is an
 `MGLStyleValue` object containing the value to use when the function is given
 the associated attribute key. An `MGLStyleFunction` object may not be used
 recursively as a value inside the nested dictionary.
 */
@property (nonatomic, copy) NS_DICTIONARY_OF(id, NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *) *stops;

/**
 An `MGLStyleValue` object containing the default value to use when there is
 no input to provide to the function.
 */
@property (nonatomic, nullable) MGLStyleValue<T> *defaultValue;

@end

NS_ASSUME_NONNULL_END
