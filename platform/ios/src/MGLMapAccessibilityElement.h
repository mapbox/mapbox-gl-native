#import <UIKit/UIKit.h>

#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;

/// Unique identifier representing a single annotation in mbgl.
typedef uint32_t MGLAnnotationTag;

/** An accessibility element representing something that appears on the map. */
MGL_EXPORT
@interface MGLMapAccessibilityElement : UIAccessibilityElement

@end

/** An accessibility element representing a map annotation. */
@interface MGLAnnotationAccessibilityElement : MGLMapAccessibilityElement

/** The tag of the annotation represented by this element. */
@property (nonatomic) MGLAnnotationTag tag;

- (instancetype)initWithAccessibilityContainer:(id)container tag:(MGLAnnotationTag)identifier NS_DESIGNATED_INITIALIZER;

@end

/** An accessibility element representing a map feature. */
MGL_EXPORT
@interface MGLFeatureAccessibilityElement : MGLMapAccessibilityElement

/** The feature represented by this element. */
@property (nonatomic, strong) id <MGLFeature> feature;

- (instancetype)initWithAccessibilityContainer:(id)container feature:(id <MGLFeature>)feature NS_DESIGNATED_INITIALIZER;

@end

/** An accessibility element representing a place feature. */
MGL_EXPORT
@interface MGLPlaceFeatureAccessibilityElement : MGLFeatureAccessibilityElement
@end

/** An accessibility element representing a road feature. */
MGL_EXPORT
@interface MGLRoadFeatureAccessibilityElement : MGLFeatureAccessibilityElement
@end

/** An accessibility element representing the MGLMapView at large. */
MGL_EXPORT
@interface MGLMapViewProxyAccessibilityElement : UIAccessibilityElement
@end

NS_ASSUME_NONNULL_END
