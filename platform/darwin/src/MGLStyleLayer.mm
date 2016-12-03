#import "MGLStyleLayer_Private.h"
#import "MGLMapView_Private.h"

#include <mbgl/style/layer.hpp>

@implementation MGLStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super init]) {
        _identifier = identifier;
    }
    return self;
}

- (void)setVisible:(BOOL)visible
{
    MGLAssertStyleLayerIsValid();

    mbgl::style::VisibilityType v = visible
    ? mbgl::style::VisibilityType::Visible
    : mbgl::style::VisibilityType::None;
    self.rawLayer->setVisibility(v);
}

- (BOOL)isVisible
{
    MGLAssertStyleLayerIsValid();

    mbgl::style::VisibilityType v = self.rawLayer->getVisibility();
    return (v == mbgl::style::VisibilityType::Visible);
}

- (void)setMaximumZoomLevel:(float)maximumZoomLevel
{
    MGLAssertStyleLayerIsValid();

    self.rawLayer->setMaxZoom(maximumZoomLevel);
}

- (float)maximumZoomLevel
{
    MGLAssertStyleLayerIsValid();

    return self.rawLayer->getMaxZoom();
}

- (void)setMinimumZoomLevel:(float)minimumZoomLevel
{
    MGLAssertStyleLayerIsValid();

    self.rawLayer->setMinZoom(minimumZoomLevel);
}

- (float)minimumZoomLevel
{
    MGLAssertStyleLayerIsValid();
    
    return self.rawLayer->getMinZoom();
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@; visible = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier,
            self.visible ? @"YES" : @"NO"];
}

@end
