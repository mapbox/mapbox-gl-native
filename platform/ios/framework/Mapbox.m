#import <Mapbox/Mapbox.h>

#import "NSBundle+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSString+MGLAdditions.h"

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
    [MGLOfflinePack class];
    [MGLOfflineStorage class];
    [MGLPointAnnotation class];
    [MGLPolygon class];
    [MGLPolyline class];
    [MGLShape class];
    [MGLStyle class];
    [MGLTilePyramidOfflineRegion class];
    [MGLUserLocation class];
}
