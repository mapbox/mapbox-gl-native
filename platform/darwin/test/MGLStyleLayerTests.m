#import "MGLStyleLayerTests.h"

@implementation MGLStyleLayerTests

- (void)setUp {
    [super setUp];
#if TARGET_OS_IPHONE
    UIApplication *app = [[UIApplication sharedApplication] delegate];
    UIViewController *vc = [[UIViewController alloc] init];
    app.keyWindow.rootViewController = vc;
    [vc view]; // Force load xib
    _mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 256, 256)];
    [vc.view addSubview:_mapView];
    _mapView.delegate = self;
#else
    NSWindowController *windowController = [[NSWindowController alloc] initWithWindowNibName:@"MGLStyleLayerTests" owner:self];
    [windowController showWindow:nil];
#endif
}

@end
