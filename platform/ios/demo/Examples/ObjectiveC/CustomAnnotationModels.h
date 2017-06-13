@import Mapbox;

// MGLAnnotation protocol reimplementation
@interface CustomPointAnnotation : NSObject <MGLAnnotation>

// As a reimplementation of the MGLAnnotation protocol, we have to add mutable coordinate and (sub)title properties ourselves.
@property (nonatomic, assign) CLLocationCoordinate2D coordinate;
@property (nonatomic, copy, nullable) NSString *title;
@property (nonatomic, copy, nullable) NSString *subtitle;

// Custom properties that we will use to customize the annotation's image.
@property (nonatomic, copy, nonnull) UIImage *image;
@property (nonatomic, copy, nonnull) NSString *reuseIdentifier;

@end
@implementation CustomPointAnnotation
@end

// MGLPolyline subclass
@interface CustomPolyline : MGLPolyline

// Because this is a subclass of MGLPolyline, there is no need to redeclare its properties.

// Custom property that we will use when drawing the polyline.
@property (nonatomic, strong, nullable) UIColor *color;

@end
@implementation CustomPolyline
@end
