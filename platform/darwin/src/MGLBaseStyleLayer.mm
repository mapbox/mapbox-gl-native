#import "MGLBaseStyleLayer.h"

#include <mbgl/style/layer.hpp>

@interface MGLBaseStyleLayer()
@property (nonatomic) mbgl::style::Layer *layer;
@end

@implementation MGLBaseStyleLayer

- (void)setVisible:(BOOL)visible
{
    mbgl::style::VisibilityType v = visible ? mbgl::style::VisibilityType::Visible : mbgl::style::VisibilityType::None;
    _layer->setVisibility(v);
}

- (BOOL)isVisible
{
    mbgl::style::VisibilityType v = _layer->getVisibility();
    return (v == mbgl::style::VisibilityType::Visible);
}

- (void)setMaxZoom:(float)maxZoom
{
    _layer->setMaxZoom(maxZoom);
}

- (float)maxZoom
{
    return _layer->getMaxZoom();
}

- (void)setMinZoom:(float)minZoom
{
    _layer->setMinZoom(minZoom);
}

- (float)minZoom
{
    return _layer->getMinZoom();
}

@end
