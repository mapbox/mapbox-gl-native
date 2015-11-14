#import "NSBundle+MGLAdditions.h"

#import "MGLMapView.h"

@implementation NSBundle (MGLAdditions)

void mgl_linkBundleCategory(){}

+ (NSString *)mgl_resourceBundlePath
{
    NSString *resourceBundlePath = nil;

    // check for resource bundle in framework bundle (Fabric, premade framework)
    //
    NSString *frameworkBundlePath = [NSString stringWithFormat:@"%@/Mapbox.framework/Mapbox.bundle",
        [[NSBundle mainBundle] privateFrameworksPath]];
    if ([NSBundle bundleWithPath:frameworkBundlePath]) resourceBundlePath = frameworkBundlePath;

    // check for resource bundle in app bundle (static library)
    //
    if ( ! resourceBundlePath) resourceBundlePath = [[NSBundle bundleForClass:
        [MGLMapView class]] pathForResource:@"Mapbox" ofType:@"bundle"];

    // fall back to resources directly in app bundle (test app)
    //
    if ( ! resourceBundlePath) resourceBundlePath = [[NSBundle mainBundle] bundlePath];

    return resourceBundlePath;
}

@end
