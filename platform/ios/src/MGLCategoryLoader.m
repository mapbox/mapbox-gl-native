#import "MGLCategoryLoader.h"

#import "NSBundle+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSString+MGLAdditions.h"

#import "MGLMapView.h"

@implementation MGLCategoryLoader

+ (void)loadCategories
{
    // https://github.com/mapbox/mapbox-gl-native/issues/2966
    //
    mgl_linkBundleCategory();
    mgl_linkProcessCategory();
    mgl_linkStringCategory();

    // https://github.com/mapbox/mapbox-gl-native/issues/3113
    //
    NSString *description = [MGLMapView description];
}

@end
