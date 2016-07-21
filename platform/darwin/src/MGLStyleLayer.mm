#import "MGLStyleLayer.h"

#import "MGLTypes.h"
#import "MGLStyleLayer_Private.hpp"
#import "MGLMapView_Private.hpp"

@interface MGLStyleLayer()
@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic) mbgl::style::Layer *layer;
@end

@implementation MGLStyleLayer

- (void)updateStyleAndClasses
{
    self.mapView.mbglMap->update(mbgl::Update::RecalculateStyle | mbgl::Update::Classes);
}

@end
