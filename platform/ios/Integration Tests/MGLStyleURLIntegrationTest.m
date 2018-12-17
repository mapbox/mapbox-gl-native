#import "MGLMapViewIntegrationTest.h"

@interface MGLStyleURLIntegrationTest : MGLMapViewIntegrationTest
@end

@implementation MGLStyleURLIntegrationTest

- (void)testLoadingStreetsStyleURL {
    self.mapView.styleURL = [MGLStyle streetsStyleURL];
    [self waitForMapViewToFinishLoadingStyleWithTimeout:5];
}

- (void)testLoadingOutdoorsStyleURL {
    self.mapView.styleURL = [MGLStyle outdoorsStyleURL];
    [self waitForMapViewToFinishLoadingStyleWithTimeout:5];
}

- (void)testLoadingLightStyleURL {
    self.mapView.styleURL = [MGLStyle lightStyleURL];
    [self waitForMapViewToFinishLoadingStyleWithTimeout:5];
}

- (void)testLoadingDarkStyleURL {
    self.mapView.styleURL = [MGLStyle darkStyleURL];
    [self waitForMapViewToFinishLoadingStyleWithTimeout:5];
}

- (void)testLoadingSatelliteStyleURL {
    self.mapView.styleURL = [MGLStyle satelliteStyleURL];
    [self waitForMapViewToFinishLoadingStyleWithTimeout:5];
}

- (void)testLoadingSatelliteStreetsStyleURL {
    self.mapView.styleURL = [MGLStyle satelliteStreetsStyleURL];
    [self waitForMapViewToFinishLoadingStyleWithTimeout:5];
}

@end
