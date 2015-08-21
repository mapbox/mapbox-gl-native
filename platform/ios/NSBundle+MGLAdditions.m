#import "NSBundle+MGLAdditions.h"

#import "MGLMapView.h"

@implementation NSBundle (MGLAdditions)

+ (NSString *)mgl_resourceBundlePath
{
    NSString *resourceBundlePath = [[NSBundle bundleForClass:[MGLMapView class]] pathForResource:@"Mapbox" ofType:@"bundle"];

    if ( ! resourceBundlePath) resourceBundlePath = [[NSBundle mainBundle] bundlePath];

    return resourceBundlePath;
}

@end
