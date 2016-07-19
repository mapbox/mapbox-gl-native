#import "MGLStyleLayer.h"

#import "MGLTypes.h"
#import "MGLStyleLayer_Private.hpp"
#include <mbgl/mbgl.hpp>

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

@interface MGLStyleLayer()
@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic) mbgl::Map *mbglMap;
@property (nonatomic) mbgl::style::Layer *layer;
@end

@implementation MGLStyleLayer

mbgl::Color MGLColorFromColor(MGLColor *color)
{
    if (!color)
    {
        return { 0, 0, 0, 0 };
    }
    CGFloat r, g, b, a;
    [color getRed:&r green:&g blue:&b alpha:&a];
    
    return { (float)r, (float)g, (float)b, (float)a };
}

- (void)updateStyleAndClasses
{
    self.mbglMap->update(mbgl::Update::RecalculateStyle | mbgl::Update::Classes);
}

@end
