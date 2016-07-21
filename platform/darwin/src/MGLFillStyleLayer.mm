#import "MGLFillStyleLayer.h"

#include "MGLStyleLayer_Private.hpp"
#include <mbgl/style/layers/fill_layer.hpp>

@interface MGLFillStyleLayer() {
    mbgl::style::FillLayer *fillLayer;
}
@end

@implementation MGLFillStyleLayer

- (void)setLayer:(mbgl::style::Layer *)layer
{
    fillLayer = reinterpret_cast<mbgl::style::FillLayer *>(layer);
}

- (void)setFillColor:(MGLColor *)fillColor
{
    _fillColor = fillColor;
    fillLayer->setFillColor(fillColor.mbgl_color);
    [self updateStyleAndClasses];
}

@end
