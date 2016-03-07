#import <Mapbox/Mapbox.h>

#import "../src/NSBundle+MGLAdditions.h"
#import "../src/NSProcessInfo+MGLAdditions.h"
#import "../../darwin/src/NSString+MGLAdditions.h"

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
    [MGLOfflineStorage class];
    [MGLOfflineTask class];
    [MGLPointAnnotation class];
    [MGLPolygon class];
    [MGLPolyline class];
    [MGLShape class];
    [MGLStyle class];
    [MGLTilePyramidOfflineRegion class];
    [MGLUserLocation class];
}
