#import "MGLBaseStyleLayer.h"

#include <mbgl/style/layer.hpp>

@interface MGLBaseStyleLayer()
@property (nonatomic) mbgl::style::Layer *layer;
@end

@implementation MGLBaseStyleLayer

- (void)setVisible:(BOOL)visible
{
    mbgl::style::VisibilityType v = visible ? mbgl::style::VisibilityType::Visible : mbgl::style::VisibilityType::None;
    self.layer->setVisibility(v);
}

- (BOOL)isVisible
{
    mbgl::style::VisibilityType v = self.layer->getVisibility();
    return (v == mbgl::style::VisibilityType::Visible);
}

- (void)setMaxZoom:(float)maxZoom
{
    self.layer->setMaxZoom(maxZoom);
}

- (float)maxZoom
{
    return self.layer->getMaxZoom();
}

- (void)setMinZoom:(float)minZoom
{
    self.layer->setMinZoom(minZoom);
}

- (float)minZoom
{
    return self.layer->getMinZoom();
}

@end
