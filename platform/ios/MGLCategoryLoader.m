#import "MGLCategoryLoader.h"

#import "NSBundle+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSString+MGLAdditions.h"

// https://github.com/mapbox/mapbox-gl-native/issues/2966

@implementation MGLCategoryLoader

+ (void)loadCategories
{
    mgl_linkBundleCategory();
    mgl_linkProcessCategory();
    mgl_linkStringCategory();
}

@end
