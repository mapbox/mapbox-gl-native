#import <Mapbox/Mapbox.h>

MGL_EXPORT

@interface MGLPinAnnotationView : MGLAnnotationView

@property(nonatomic) UIColor *shadowColor;
@property(nonatomic) UIColor *pinColor;
@property(nonatomic) UIColor *strokeColor;
@property(nonatomic) UIColor *innerColor;

@end
