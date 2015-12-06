#import <Mapbox/Mapbox.h>

#import "../src/MGLMapView_Private.h"
#import "../src/NSBundle+MGLAdditions.h"
#import "../src/NSProcessInfo+MGLAdditions.h"
#import "../../darwin/NSString+MGLAdditions.h"

__attribute__((constructor))
static void InitializeMapbox() {
    static int initialized = 0;
    if (initialized) {
        return;
    }
    
    mgl_linkBundleCategory();
    mgl_linkStringCategory();
    mgl_linkProcessInfoCategory();
    mgl_linkMapViewIBCategory();
    
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
}
