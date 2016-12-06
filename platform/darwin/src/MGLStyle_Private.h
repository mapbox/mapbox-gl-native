#import "MGLStyle.h"

#import "MGLStyleLayer.h"
#import "MGLFillStyleLayer.h"
#import <mbgl/util/default_styles.hpp>
#include <mbgl/mbgl.hpp>

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;
@class MGLOpenGLStyleLayer;

@interface MGLStyle (Private)

- (instancetype)initWithMapView:(MGLMapView *)mapView;

@property (nonatomic, readonly, weak) MGLMapView *mapView;

@property (nonatomic, readonly, strong) NS_MUTABLE_DICTIONARY_OF(NSString *, MGLOpenGLStyleLayer *) *openGLLayers;

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration;

@end

NS_ASSUME_NONNULL_END
