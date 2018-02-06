#import <Mapbox/Mapbox.h>

/**
 The MGLPinAnnotation class defines a view-based annotation that conforms to the`MGLAnnotation` protocol. This class provides initializers to create a pin-shaped annotation with customizable colors.
 */

MGL_EXPORT
@interface MGLPinAnnotationView : MGLAnnotationView

#pragma mark Configuring appearance

/** A `UIColor` that defines the color of the pin's shadow. */
@property(nonatomic) UIColor *shadowColor;

/** A `UIColor` that defines the pin's primary color. */
@property(nonatomic) UIColor *pinColor;

/** A `UIColor` that defines the color of the pin's stroke outline. */
@property(nonatomic) UIColor *strokeColor;

/** A `UIColor` that defines the color of the pin's inner circle. */
@property(nonatomic) UIColor *innerColor;

@end
