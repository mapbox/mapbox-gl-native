#import <mbgl/osx/Mapbox.h>

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
    
    [MGLAccountManager class];
    [MGLAnnotationImage class];
    [MGLMapView class];
    [MGLMultiPoint class];
    [MGLPointAnnotation class];
    [MGLPolygon class];
    [MGLPolyline class];
    [MGLShape class];
    [MGLStyle class];
}
