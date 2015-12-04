#import <Mapbox/Mapbox.h>

#import "../../platform/ios/src/NSBundle+MGLAdditions.h"
#import "../../platform/ios/src/NSProcessInfo+MGLAdditions.h"
#import "../../platform/darwin/NSString+MGLAdditions.h"

__attribute__((constructor))
static void InitializeMapbox() {
    static int initialized = 0;
    if (initialized) {
        return;
    }
    
    mgl_linkBundleCategory();
    mgl_linkStringCategory();
    mgl_linkProcessInfoCategory();
    
    [MGLAccountManager class];
    [MGLAnnotationImage class];
    [MGLMapCamera class];
    [MGLMapView class];
    [MGLMultiPoint class];
    [MGLPointAnnotation class];
    [MGLPolygon class];
    [MGLPolyline class];
    [MGLShape class];
    [MGLStyle class];
    [MGLUserLocation class];
}
