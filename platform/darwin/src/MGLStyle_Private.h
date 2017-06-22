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

@interface MGLStyle (Private)

- (instancetype)initWithRawStyle:(mbgl::style::Style *)rawStyle mapView:(MGLMapView *)mapView;

@property (nonatomic, readonly, weak) MGLMapView *mapView;
@property (nonatomic, readonly) mbgl::style::Style *rawStyle;

- (nullable NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosWithFontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor;

@property (nonatomic, readonly, strong) NS_MUTABLE_DICTIONARY_OF(NSString *, MGLOpenGLStyleLayer *) *openGLLayers;

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration;

@end

NS_ASSUME_NONNULL_END
