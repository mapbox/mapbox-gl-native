#import "MGLStyle.h"

#import "MGLStyleLayer.h"
#import "MGLFillStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    namespace style {
        class Style;
    }
}

@class MGLAttributionInfo;
@class MGLMapView;
@class MGLOpenGLStyleLayer;
@class MGLVectorSource;
@class MGLVectorStyleLayer;

@interface MGLStyle (Private)

- (instancetype)initWithRawStyle:(mbgl::style::Style *)rawStyle mapView:(MGLMapView *)mapView;

@property (nonatomic, readonly, weak) MGLMapView *mapView;
@property (nonatomic, readonly) mbgl::style::Style *rawStyle;
@property (nonatomic, readonly) NSMutableSet *managedLayers;

- (nullable NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosWithFontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor;

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration;

- (void)addToManagedLayers:(MGLStyleLayer*)layer;
- (void)removeFromManagedLayers:(MGLStyleLayer*)layer;
@end

@interface MGLStyle (MGLStreetsAdditions)

@property (nonatomic, readonly, copy) NS_ARRAY_OF(MGLVectorStyleLayer *) *placeStyleLayers;
@property (nonatomic, readonly, copy) NS_ARRAY_OF(MGLVectorStyleLayer *) *roadStyleLayers;

@end

NS_ASSUME_NONNULL_END
