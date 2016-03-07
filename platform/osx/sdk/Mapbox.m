#import <Mapbox/Mapbox.h>

#import "../src/MGLMapView_Private.h"
#import "../../darwin/src/NSBundle+MGLAdditions.h"
#import "../../darwin/src/NSProcessInfo+MGLAdditions.h"
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
    mgl_linkMapViewIBCategory();
    
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
}
