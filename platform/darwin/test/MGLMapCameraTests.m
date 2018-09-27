#import <XCTest/XCTest.h>
#import <CoreLocation/CoreLocation.h>
#import <Mapbox/Mapbox.h>
#import <MapKit/MapKit.h>

@interface MGLMapCameraTests : XCTestCase

@end

@implementation MGLMapCameraTests

- (void)testEyeCoordinateInitialization {
    CLLocationCoordinate2D fountainSquare = CLLocationCoordinate2DMake(39.10152215, -84.5124439696089);
    CLLocationCoordinate2D unionTerminal = CLLocationCoordinate2DMake(39.10980955, -84.5352778794236);
    
    MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                                       fromEyeCoordinate:fountainSquare
                                                             eyeAltitude:1000];
    MKMapCamera *mkCamera = [MKMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                                       fromEyeCoordinate:fountainSquare
                                                             eyeAltitude:1000];
    XCTAssertEqual(camera.centerCoordinate.latitude, fountainSquare.latitude);
    XCTAssertEqual(camera.centerCoordinate.longitude, fountainSquare.longitude);
    XCTAssertEqual(camera.centerCoordinate.latitude, mkCamera.centerCoordinate.latitude);
    XCTAssertEqual(camera.centerCoordinate.longitude, mkCamera.centerCoordinate.longitude);
    XCTAssertEqual(camera.altitude, 1000, @"Eye altitude should be equivalent to altitude in untilted camera.");
    XCTAssertEqual(camera.altitude, mkCamera.altitude, @"Eye altitude in untilted camera should match MapKit.");
    XCTAssertEqual(camera.pitch, 0, @"Camera directly over center coordinate should be untilted.");
    XCTAssertEqual(camera.pitch, mkCamera.pitch, @"Camera directly over center coordinate should have same pitch as MapKit.");
    XCTAssertEqual(camera.heading, 0, @"Camera directly over center coordinate should be unrotated.");
    XCTAssertEqual(camera.heading, mkCamera.heading, @"Camera directly over center coordinate should have same heading as MapKit.");
    
    camera = [MGLMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                         fromEyeCoordinate:unionTerminal
                                               eyeAltitude:1000];
    mkCamera = [MKMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                          fromEyeCoordinate:unionTerminal
                                                eyeAltitude:1000];
    XCTAssertEqual(camera.centerCoordinate.latitude, fountainSquare.latitude);
    XCTAssertEqual(camera.centerCoordinate.longitude, fountainSquare.longitude);
    XCTAssertEqual(camera.centerCoordinate.latitude, mkCamera.centerCoordinate.latitude);
    XCTAssertEqual(camera.centerCoordinate.longitude, mkCamera.centerCoordinate.longitude);
    XCTAssertEqual(camera.altitude, 1000);
    XCTAssertEqual(camera.altitude, mkCamera.altitude, @"Eye altitude in tilted camera should match MapKit.");
    XCTAssertEqualWithAccuracy(camera.pitch, 65.3469146074, 0.01);
    XCTAssertEqual(camera.pitch, mkCamera.pitch);
    XCTAssertEqualWithAccuracy(camera.heading, 115.066396383, 0.01);
    XCTAssertEqualWithAccuracy(camera.heading, mkCamera.heading, 0.01);
}

- (void)testViewingDistanceInitialization {
    CLLocationCoordinate2D fountainSquare = CLLocationCoordinate2DMake(39.10152215, -84.5124439696089);
    MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                                          acrossDistance:10000
                                                                   pitch:0
                                                                 heading:0];
    MKMapCamera *mkCamera = [MKMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                                            fromDistance:10000
                                                                   pitch:0
                                                                 heading:0];
    XCTAssertEqualWithAccuracy(camera.altitude, 10000, 0.01, @"Untilted camera should use distance verbatim.");
    XCTAssertEqualWithAccuracy(camera.altitude, mkCamera.altitude, 0.01, @"Untilted camera altitude should match MapKit.");
    
    camera = [MGLMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                                  altitude:10000
                                                     pitch:0
                                                   heading:0];
    XCTAssertEqual(camera.altitude, 10000, @"Untilted camera should use altitude verbatim.");
    
    camera = [MGLMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                            acrossDistance:10000
                                                     pitch:60
                                                   heading:0];
    mkCamera = [MKMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                               fromDistance:10000
                                                      pitch:60
                                                    heading:0];
    XCTAssertEqualWithAccuracy(camera.altitude, 5000, 0.01, @"Tilted camera altitude should account for pitch.");
    XCTAssertEqualWithAccuracy(camera.altitude, mkCamera.altitude, 0.01, @"Tilted camera altitude should match MapKit.");
    
    camera = [MGLMapCamera cameraLookingAtCenterCoordinate:fountainSquare
                                                  altitude:10000
                                                     pitch:60
                                                   heading:0];
    XCTAssertEqual(camera.altitude, 10000, @"Tilted camera should use altitude verbatim.");
}

- (void)testViewingDistance {
    MGLMapCamera *camera = [MGLMapCamera camera];
    camera.altitude = 10000;
    XCTAssertEqual(camera.altitude, 10000);
    XCTAssertEqual(camera.viewingDistance, 10000);
    camera.viewingDistance = 10000;
    XCTAssertEqual(camera.altitude, 10000);
    XCTAssertEqual(camera.viewingDistance, 10000);
    
    camera.pitch = 60;
    camera.altitude = 10000;
    XCTAssertEqual(camera.altitude, 10000);
    XCTAssertEqualWithAccuracy(camera.viewingDistance, 20000, 0.01);
    camera.viewingDistance = 10000;
    XCTAssertEqualWithAccuracy(camera.altitude, 5000, 0.01);
    XCTAssertEqual(camera.viewingDistance, 10000);
}

@end
