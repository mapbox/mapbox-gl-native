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
@class MGLVectorTileSource;
@class MGLVectorStyleLayer;

@interface MGLStyle (Private)

- (instancetype)initWithRawStyle:(mbgl::style::Style *)rawStyle mapView:(MGLMapView *)mapView;

@property (nonatomic, readonly, weak) MGLMapView *mapView;
@property (nonatomic, readonly) mbgl::style::Style *rawStyle;

- (nullable NSArray<MGLAttributionInfo *> *)attributionInfosWithFontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor;
@property (nonatomic, readonly, strong) NSMutableDictionary<NSString *, MGLOpenGLStyleLayer *> *openGLLayers;
- (void)setStyleClasses:(NSArray<NSString *> *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration;

@end

@interface MGLStyle (MGLStreetsAdditions)

@property (nonatomic, readonly, copy) NSArray<MGLVectorStyleLayer *> *placeStyleLayers;
@property (nonatomic, readonly, copy) NSArray<MGLVectorStyleLayer *> *roadStyleLayers;

@end

NS_ASSUME_NONNULL_END
