#import <Foundation/Foundation.h>

#import "MGLFoundation.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 `MGLStyleLayer` is an abstract base class for style layers. A style layer
 manages the layout and appearance of content at a specific z-index in a style.
 An `MGLStyle` object consists of one or more `MGLStyleLayer` objects.

 Each style layer defined by the style JSON file is represented at runtime by an
 `MGLStyleLayer` object, which you can use to refine the map’s appearance. You
 can also add and remove style layers dynamically.

 Do not create instances of this class directly, and do not create your own
 subclasses of this class. Instead, create instances of
 `MGLBackgroundStyleLayer` and the concrete subclasses of
 `MGLForegroundStyleLayer`.
 
 Do not add `MGLStyleLayer` objects to the `style` property of a `MGLMapView` before
 `-mapView:didFinishLoadingStyle:` is called.
 */
MGL_EXPORT
@interface MGLStyleLayer : NSObject

#pragma mark Initializing a Style Layer

- (instancetype)init __attribute__((unavailable("Use -initWithIdentifier: instead.")));

/**
 Returns a style layer object initialized with the given identifier.

 The default implementation of this initializer in MGLStyleLayer creates an
 invalid style layer. Call this initializer on `MGLBackgroundStyleLayer` or one of
 the concrete subclasses of `MGLForegroundStyleLayer` to create a valid style
 layer.

 After initializing and configuring the style layer, add it to a map view’s
 style using the `-[MGLStyle addLayer:]` or
 `-[MGLStyle insertLayer:belowLayer:]` method.

 @param identifier A string that uniquely identifies the layer in the style to
    which it is added.
 @return An initialized style layer.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier;

#pragma mark Identifying a Style Layer

/**
 A string that uniquely identifies the style layer in the style to which it is
 added.
 */
@property (nonatomic, copy, readonly) NSString *identifier;

#pragma mark Configuring a Style Layer’s Visibility

/**
 Whether this layer is displayed. A value of `NO` hides the layer.
 */
@property (nonatomic, assign, getter=isVisible) BOOL visible;

/**
 The maximum zoom level at which the layer gets parsed and appears. This value is a floating-point number.
 */
@property (nonatomic, assign) float maximumZoomLevel;

/**
 The minimum zoom level at which the layer gets parsed and appears. This value is a floating-point number.
 */
@property (nonatomic, assign) float minimumZoomLevel;

@end

NS_ASSUME_NONNULL_END
