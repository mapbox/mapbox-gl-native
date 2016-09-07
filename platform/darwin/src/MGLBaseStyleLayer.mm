#import "MGLBaseStyleLayer.h"

#import "MGLStyleLayer_Private.h"
#import "MGLMapView_Private.h"

#include <mbgl/style/layer.hpp>

@interface MGLBaseStyleLayer() <MGLStyleLayer_Private>
@end

@implementation MGLBaseStyleLayer

@synthesize layerIdentifier;
@synthesize mapView;
@synthesize layer;
@synthesize sourceIdentifier;
@synthesize sourceLayerIdentifier;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier
{
    [[NSException exceptionWithName:@"MGLAbstractClassException"
                             reason:@"MGLBaseStyleLayer is an abstract class"
                           userInfo:nil] raise];
    return nil;
}

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier sourceIdentifier:(NSString *)sourceIdentifier
{
    [[NSException exceptionWithName:@"MGLAbstractClassException"
                             reason:@"MGLBaseStyleLayer is an abstract class"
                           userInfo:nil] raise];
    return nil;
}

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier sourceIdentifier:(NSString *)sourceIdentifier sourceLayer:(NSString *)sourceLayer
{
    [[NSException exceptionWithName:@"MGLAbstractClassException"
                             reason:@"MGLBaseStyleLayer is an abstract class"
                           userInfo:nil] raise];
    return nil;
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
