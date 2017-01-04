#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

#import "MGLFoundation.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

// TODO: API docs
typedef NSString *MGLStyleFunctionOption NS_STRING_ENUM;

// TODO: API docs
extern MGL_EXPORT const MGLStyleFunctionOption MGLStyleFunctionOptionInterpolationBase;

// TODO: API docs
extern MGL_EXPORT const MGLStyleFunctionOption MGLStyleFunctionOptionDefaultValue;

// TODO: API docs
typedef NS_ENUM(NSUInteger, MGLInterpolationMode) {
    MGLInterpolationModeExponential = 0,
    MGLInterpolationModeInterval,
    MGLInterpolationModeCategorical,
    MGLInterpolationModeIdentity
};

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
MGL_EXPORT
@interface MGLStyleValue<T> : NSObject

#pragma mark Creating a Style Value

/**
 Creates and returns an `MGLStyleConstantValue` object containing a raw value.

 @param rawValue The constant value contained by the object.
 @return An `MGLStyleConstantValue` object containing `rawValue`, which is
    treated as a constant value.
 */
+ (instancetype)valueWithRawValue:(T)rawValue;

#pragma mark Function values

/**
 Creates and returns an `MGLStyleFunction` object representing a linear zoom
 level function with any number of stops.

 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLStyleFunction` object with the given stops.
 */
+ (instancetype)valueWithStops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops __attribute__((deprecated("Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:]")));

/**
 Creates and returns an `MGLStyleFunction` object representing a zoom level
 function with an exponential interpolation base and any number of stops.

 @param interpolationBase The exponential base of the interpolation curve.
 @param stops A dictionary associating zoom levels with style values.
 @return An `MGLStyleFunction` object with the given interpolation base and stops.
 */
+ (instancetype)valueWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops __attribute__((deprecated("Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:]")));

// TODO: API docs
+ (instancetype)valueWithInterpolationMode:(MGLInterpolationMode)interpolationMode cameraStops:(NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)stops options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

// TODO: API docs
+ (instancetype)valueWithInterpolationMode:(MGLInterpolationMode)interpolationMode sourceStops:(nullable NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)stops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

// TODO: API docs
+ (instancetype)valueWithInterpolationMode:(MGLInterpolationMode)interpolationMode compositeStops:(NS_DICTIONARY_OF(id, NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *) *)stops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

@end

/**
 An `MGLStyleConstantValue` object is a generic container for a style attribute
 value that remains constant as the zoom level changes. The layout and paint
 attribute properties of `MGLStyleLayer` objects can be set to
 `MGLStyleConstantValue` objects.

 The `MGLStyleConstantValue` class takes a generic parameter `T` that indicates
 the Foundation class being wrapped by this class.
 */
MGL_EXPORT
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

MGL_EXPORT
@interface MGLCameraStyleFunction<T> : MGLStyleValue<T>

// TODO: API docs
+ (instancetype)functionWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)stops options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

// TODO: API docs
- (instancetype)initWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)stops options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options NS_DESIGNATED_INITIALIZER;

// TODO: API docs
@property (nonatomic) MGLInterpolationMode interpolationMode;

// TODO: API docs
@property (nonatomic, copy) NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *stops;

// TODO: API docs
@property (nonatomic) CGFloat interpolationBase;

@end

@compatibility_alias MGLStyleFunction MGLCameraStyleFunction;

MGL_EXPORT
@interface MGLSourceStyleFunction<T> : MGLStyleValue<T>

// TODO: API docs
+ (instancetype)functionWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(nullable NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)stops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

// TODO: API docs
- (instancetype)initWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(nullable NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *)stops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options NS_DESIGNATED_INITIALIZER;

// TODO: API docs
@property (nonatomic) MGLInterpolationMode interpolationMode;

// TODO: API docs
@property (nonatomic, copy) NSString *attributeName;

// TODO: API docs
@property (nonatomic, copy, nullable) NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *stops;

// TODO: API docs
@property (nonatomic, nullable) MGLStyleValue<T> *defaultValue;

// TODO: API docs
@property (nonatomic) CGFloat interpolationBase;

@end

MGL_EXPORT
@interface MGLCompositeStyleFunction<T> : MGLStyleValue<T>

// TODO: API docs
+ (instancetype)functionWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NS_DICTIONARY_OF(id, NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *) *)stops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options;

- (instancetype)initWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NS_DICTIONARY_OF(id, NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *) *)stops attributeName:(NSString *)attributeName options:(nullable NS_DICTIONARY_OF(MGLStyleFunctionOption, id) *)options NS_DESIGNATED_INITIALIZER;

// TODO: API docs
@property (nonatomic) MGLInterpolationMode interpolationMode;

// TODO: API docs
@property (nonatomic, copy) NSString *attributeName;

// TODO: API docs
@property (nonatomic, copy) NS_DICTIONARY_OF(id, NS_DICTIONARY_OF(id, MGLStyleValue<T> *) *) *stops;

// TODO: API docs
@property (nonatomic, nullable) MGLStyleValue<T> *defaultValue;

// TODO: API docs
@property (nonatomic) CGFloat interpolationBase;

@end

NS_ASSUME_NONNULL_END
