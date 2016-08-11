#import "MGLBaseStyleLayer.h"

#import "MGLStyleLayer_Private.hpp"
#import "MGLMapView_Private.hpp"

#include <mbgl/style/layer.hpp>

@interface MGLBaseStyleLayer() <MGLStyleLayer_Private>
@end

@implementation MGLBaseStyleLayer

@synthesize layerIdentifier;
@synthesize mapView;
@synthesize layer;

- (void)update
{
    self.mapView.mbglMap->update(mbgl::Update::RecalculateStyle | mbgl::Update::Classes);
}

- (void)setVisible:(BOOL)visible
{
    mbgl::style::VisibilityType v = visible
    ? mbgl::style::VisibilityType::Visible
    : mbgl::style::VisibilityType::None;
    self.layer->setVisibility(v);
}

- (BOOL)isVisible
{
    mbgl::style::VisibilityType v = self.layer->getVisibility();
    return (v == mbgl::style::VisibilityType::Visible);
}

- (void)setMaximumZoomLevel:(float)maximumZoomLevel
{
    self.layer->setMaxZoom(maximumZoomLevel);
}

- (float)maximumZoomLevel
{
    return self.layer->getMaxZoom();
}

- (void)setMinimumZoomLevel:(float)minimumZoomLevel
{
    self.layer->setMinZoom(minimumZoomLevel);
}

- (float)minimumZoomLevel
{
    return self.layer->getMinZoom();
}

@end
