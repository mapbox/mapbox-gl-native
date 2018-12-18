#import "MGLMapViewIntegrationTest.h"

@interface MGLStyleURLIntegrationTest : MGLMapViewIntegrationTest
@end

@implementation MGLStyleURLIntegrationTest

- (void)internalTestWithStyleSelector:(SEL)selector {
    if (![self validAccessToken]) {
        return;
    }

    self.mapView.styleURL = [MGLStyle performSelector:selector];
    [self waitForMapViewToFinishLoadingStyleWithTimeout:5];
}


- (void)testLoadingStreetsStyleURL {
    [self internalTestWithStyleSelector:@selector(streetsStyleURL)];
}

- (void)testLoadingOutdoorsStyleURL {
    [self internalTestWithStyleSelector:@selector(outdoorsStyleURL)];
}

- (void)testLoadingLightStyleURL {
    [self internalTestWithStyleSelector:@selector(lightStyleURL)];
}

- (void)testLoadingDarkStyleURL {
    [self internalTestWithStyleSelector:@selector(darkStyleURL)];
}

- (void)testLoadingSatelliteStyleURL {
    [self internalTestWithStyleSelector:@selector(satelliteStyleURL)];
}

- (void)testLoadingSatelliteStreetsStyleURL {
    [self internalTestWithStyleSelector:@selector(satelliteStreetsStyleURL)];
}

@end
