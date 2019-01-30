#import <XCTest/XCTest.h>
#import "MGLMapView.h"
#import "MGLMapViewDelegate.h"
#import "MGLAccountManager.h"


@interface MGLOrnamentTestData : NSObject

@property (nonatomic) MGLOrnamentPosition position;
@property (nonatomic) CGPoint offset;
@property (nonatomic) CGPoint expectedOrigin;

@end

@implementation MGLOrnamentTestData

+ (instancetype)createWithPostion:(MGLOrnamentPosition)position offset:(CGPoint)offset expectedOrigin:(CGPoint)expectedOrigin {
    MGLOrnamentTestData *data = [[MGLOrnamentTestData alloc] init];
    data.position = position;
    data.offset = offset;
    data.expectedOrigin = expectedOrigin;
    return data;
}

@end


@interface MGLMapViewLayoutTests : XCTestCase<MGLMapViewDelegate>

@property (nonatomic) UIView *superView;
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) XCTestExpectation *styleLoadingExpectation;

@end

@implementation MGLMapViewLayoutTests

- (void)setUp {
    [super setUp];

    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];

    self.superView = [[UIView alloc] initWithFrame:UIScreen.mainScreen.bounds];

    self.mapView = [[MGLMapView alloc] initWithFrame:UIScreen.mainScreen.bounds styleURL:styleURL];
    self.mapView.delegate = self;

    [self.superView addSubview:self.mapView];

    UIView *mapView = self.mapView;
    NSDictionary *bindings = NSDictionaryOfVariableBindings(mapView);
    NSArray *verticalConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[mapView]-0-|" options:0 metrics:nil views:bindings];
    NSArray *horizonatalConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[mapView]-0-|" options:0 metrics:nil views:bindings];

    [self.superView addConstraints:[verticalConstraints arrayByAddingObjectsFromArray:horizonatalConstraints]];

    self.styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
    [self waitForExpectationsWithTimeout:10 handler:nil];

    self.mapView.showsScale = YES;

    //set zoom and heading so that scale bar and compass will be shown
    [self.mapView setZoomLevel:10.0 animated:NO];
    [self.mapView.camera setHeading:12.0];

    //invoke layout
    [self.superView setNeedsLayout];
    [self.superView layoutIfNeeded];
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    XCTAssertNotNil(mapView.style);
    XCTAssertEqual(mapView.style, style);

    [self.styleLoadingExpectation fulfill];
}

- (void)tearDown {
    self.styleLoadingExpectation = nil;
    self.mapView = nil;

    [super tearDown];
}

- (void)testOrnamentPlacement {

    CGFloat margin = 8.0;
    CGFloat bottomSafeAreaInset = 0.0;
    double accuracy = 0.01;

    if (@available(iOS 11.0, *)) {
        bottomSafeAreaInset = self.mapView.safeAreaInsets.bottom;
    }
    
    //compass
    UIImageView *compassView = self.mapView.compassView;

    CGFloat expectedCompassOriginX = CGRectGetMaxX(self.mapView.bounds) - margin - CGRectGetWidth(compassView.frame);
    CGFloat expectedCompassOriginY = margin;

    XCTAssertEqualWithAccuracy(CGRectGetMinX(compassView.frame), expectedCompassOriginX, accuracy);
    XCTAssertEqualWithAccuracy(CGRectGetMinY(compassView.frame), expectedCompassOriginY, accuracy);

    //scale bar
    UIView *scaleBar = self.mapView.scaleBar;

    XCTAssertEqualWithAccuracy(CGRectGetMinX(scaleBar.frame), margin, accuracy);
    XCTAssertEqualWithAccuracy(CGRectGetMinY(scaleBar.frame), margin, accuracy);

    //attribution button
    UIButton *attributionButton = self.mapView.attributionButton;

    CGFloat expectedButtonOriginX = CGRectGetMaxX(self.mapView.bounds) - margin - CGRectGetWidth(attributionButton.frame);
    CGFloat expectedButtonOriginY = CGRectGetMaxY(self.mapView.bounds) - margin - bottomSafeAreaInset - CGRectGetHeight(attributionButton.frame);

    XCTAssertEqualWithAccuracy(CGRectGetMinX(attributionButton.frame), expectedButtonOriginX, accuracy);
    XCTAssertEqualWithAccuracy(CGRectGetMinY(attributionButton.frame), expectedButtonOriginY, accuracy);

    //mapbox logo
    UIImageView *logoView = self.mapView.logoView;

    CGFloat expectedLogoOriginX = margin;
    CGFloat expectedLogoOriginY = CGRectGetMaxY(self.mapView.bounds) - margin - bottomSafeAreaInset - CGRectGetHeight(logoView.frame);

    XCTAssertEqualWithAccuracy(CGRectGetMinX(logoView.frame), expectedLogoOriginX, accuracy);
    XCTAssertEqualWithAccuracy(CGRectGetMinY(logoView.frame), expectedLogoOriginY, accuracy);
}

- (void)testOrnamentPlacementInvalidArgument {
    XCTAssertThrows([self.mapView setCompassViewOffset:CGPointMake(-4, -4)]);
    XCTAssertThrows([self.mapView setCompassViewOffset:CGPointMake(-4, 0)]);
    XCTAssertThrows([self.mapView setCompassViewOffset:CGPointMake(0, -4)]);

    XCTAssertThrows([self.mapView setScaleBarOffset:CGPointMake(-4, -4)]);
    XCTAssertThrows([self.mapView setScaleBarOffset:CGPointMake(-4, 0)]);
    XCTAssertThrows([self.mapView setScaleBarOffset:CGPointMake(0, -4)]);

    XCTAssertThrows([self.mapView setAttributionButtonOffset:CGPointMake(-4, -4)]);
    XCTAssertThrows([self.mapView setAttributionButtonOffset:CGPointMake(-4, 0)]);
    XCTAssertThrows([self.mapView setAttributionButtonOffset:CGPointMake(0, -4)]);

    XCTAssertThrows([self.mapView setLogoViewOffset:CGPointMake(-4, -4)]);
    XCTAssertThrows([self.mapView setLogoViewOffset:CGPointMake(-4, 0)]);
    XCTAssertThrows([self.mapView setLogoViewOffset:CGPointMake(0, -4)]);
}

- (NSArray *)makeTestDataListWithView:(UIView *)view margin:(CGFloat)margin {
    CGFloat bottomSafeAreaInset = 0.0;
    if (@available(iOS 11.0, *)) {
        bottomSafeAreaInset = self.mapView.safeAreaInsets.bottom;
    }

    return @[
             [MGLOrnamentTestData createWithPostion:MGLOrnamentPositionTopLeft
                                             offset:CGPointMake(margin, margin)
                                     expectedOrigin:CGPointMake(margin, margin)],
             [MGLOrnamentTestData createWithPostion:MGLOrnamentPositionTopRight
                                             offset:CGPointMake(margin, margin)
                                     expectedOrigin:CGPointMake(CGRectGetMaxX(self.mapView.bounds) - margin - CGRectGetWidth(view.frame), 4)],
             [MGLOrnamentTestData createWithPostion:MGLOrnamentPositionBottomLeft
                                             offset:CGPointMake(margin, margin)
                                     expectedOrigin:CGPointMake(margin,  CGRectGetMaxY(self.mapView.bounds) - margin - bottomSafeAreaInset - CGRectGetHeight(view.frame))],
             [MGLOrnamentTestData createWithPostion:MGLOrnamentPositionBottomRight
                                             offset:CGPointMake(margin, margin)
                                     expectedOrigin:CGPointMake(CGRectGetMaxX(self.mapView.bounds) - margin - CGRectGetWidth(view.frame),
                                                                CGRectGetMaxY(self.mapView.bounds) - margin - bottomSafeAreaInset - CGRectGetHeight(view.frame))]
             ];
}

- (void)testCompassPlacement {
    double accuracy = 0.01;
    CGFloat margin = 4.0;

    UIView *compassView = self.mapView.compassView;
    NSArray *testDataList = [self makeTestDataListWithView:compassView margin:margin];

    for (MGLOrnamentTestData *testData in testDataList) {
        self.mapView.compassViewPosition = testData.position;
        self.mapView.compassViewOffset = testData.offset;

        //invoke layout
        [self.superView setNeedsLayout];
        [self.superView layoutIfNeeded];

        XCTAssertEqualWithAccuracy(CGRectGetMinX(compassView.frame), testData.expectedOrigin.x, accuracy);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(compassView.frame), testData.expectedOrigin.y, accuracy);
    }
}

- (void)testScalebarPlacement {
    CGFloat bottomSafeAreaInset = 0.0;
    double accuracy = 0.01;
    CGFloat margin = 4.0;

    if (@available(iOS 11.0, *)) {
        bottomSafeAreaInset = self.mapView.safeAreaInsets.bottom;
    }

    UIView *scaleBar = self.mapView.scaleBar;
    NSArray *testDataList = [self makeTestDataListWithView:scaleBar margin:margin];

    for (MGLOrnamentTestData *testData in testDataList) {
        self.mapView.scaleBarPosition = testData.position;
        self.mapView.scaleBarOffset = testData.offset;

        //invoke layout
        [self.superView setNeedsLayout];
        [self.superView layoutIfNeeded];

        XCTAssertEqualWithAccuracy(CGRectGetMinX(scaleBar.frame), testData.expectedOrigin.x, accuracy);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(scaleBar.frame), testData.expectedOrigin.y, accuracy);
    }
}

- (void)testAttributionButtonPlacement {
    CGFloat bottomSafeAreaInset = 0.0;
    double accuracy = 0.01;
    CGFloat margin = 4.0;

    if (@available(iOS 11.0, *)) {
        bottomSafeAreaInset = self.mapView.safeAreaInsets.bottom;
    }

    UIView *attributionButton = self.mapView.attributionButton;
    NSArray *testDataList = [self makeTestDataListWithView:attributionButton margin:margin];

    for (MGLOrnamentTestData *testData in testDataList) {
        self.mapView.attributionButtonPosition = testData.position;
        self.mapView.attributionButtonOffset = testData.offset;

        //invoke layout
        [self.superView setNeedsLayout];
        [self.superView layoutIfNeeded];

        XCTAssertEqualWithAccuracy(CGRectGetMinX(attributionButton.frame), testData.expectedOrigin.x, accuracy);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(attributionButton.frame), testData.expectedOrigin.y, accuracy);
    }
}

- (void)testLogoPlacement {
    CGFloat bottomSafeAreaInset = 0.0;
    double accuracy = 0.01;
    CGFloat margin = 4.0;

    if (@available(iOS 11.0, *)) {
        bottomSafeAreaInset = self.mapView.safeAreaInsets.bottom;
    }

    UIView *logoView = self.mapView.logoView;
    NSArray *testDataList = [self makeTestDataListWithView:logoView margin:margin];

    for (MGLOrnamentTestData *testData in testDataList) {
        self.mapView.logoViewPosition = testData.position;
        self.mapView.logoViewOffset = testData.offset;

        //invoke layout
        [self.superView setNeedsLayout];
        [self.superView layoutIfNeeded];

        XCTAssertEqualWithAccuracy(CGRectGetMinX(logoView.frame), testData.expectedOrigin.x, accuracy);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(logoView.frame), testData.expectedOrigin.y, accuracy);
    }
}

@end
