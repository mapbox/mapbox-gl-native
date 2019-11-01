#import <XCTest/XCTest.h>
#import "MGLMapView.h"
#import "MGLMapViewDelegate.h"
#import "MGLAccountManager.h"

#import "MGLScaleBar.h"

@interface MGLOrnamentTestData : NSObject

@property (nonatomic) MGLOrnamentPosition position;
@property (nonatomic) CGPoint offset;
@property (nonatomic) CGPoint expectedOrigin;

@end

@implementation MGLOrnamentTestData

+ (instancetype)createWithPosition:(MGLOrnamentPosition)position offset:(CGPoint)offset expectedOrigin:(CGPoint)expectedOrigin {
    MGLOrnamentTestData *data = [[MGLOrnamentTestData alloc] init];
    data.position = position;
    data.offset = offset;
    data.expectedOrigin = expectedOrigin;
    return data;
}

@end

@interface MGLScaleBar (Tests)
@property (nonatomic, readonly) NSArray<UIView *> *labelViews;
@property (nonatomic, readonly) NSArray<UIView *> *bars;
@property (nonatomic, readonly) UIView *containerView;
@property (nonatomic, readonly) CGSize size;
@property (nonatomic) NSNumber *testingRightToLeftOverride;
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
    [MGLAccountManager setAccessToken:nil];

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

- (NSArray *)makeTestDataListWithView:(UIView *)view margin:(CGFloat)margin {
    CGFloat bottomSafeAreaInset = 0.0;
    if (@available(iOS 11.0, *)) {
        bottomSafeAreaInset = self.mapView.safeAreaInsets.bottom;
    }

    return @[
             [MGLOrnamentTestData createWithPosition:MGLOrnamentPositionTopLeft
                                              offset:CGPointMake(margin, margin)
                                      expectedOrigin:CGPointMake(margin, margin)],
             [MGLOrnamentTestData createWithPosition:MGLOrnamentPositionTopRight
                                              offset:CGPointMake(margin, margin)
                                      expectedOrigin:CGPointMake(CGRectGetMaxX(self.mapView.bounds) - margin - CGRectGetWidth(view.frame), margin)],
             [MGLOrnamentTestData createWithPosition:MGLOrnamentPositionBottomLeft
                                              offset:CGPointMake(margin, margin)
                                      expectedOrigin:CGPointMake(margin,  CGRectGetMaxY(self.mapView.bounds) - margin - bottomSafeAreaInset - CGRectGetHeight(view.frame))],
             [MGLOrnamentTestData createWithPosition:MGLOrnamentPositionBottomRight
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
        self.mapView.compassViewMargins = testData.offset;

        //invoke layout
        [self.superView setNeedsLayout];
        [self.superView layoutIfNeeded];

        XCTAssertEqualWithAccuracy(CGRectGetMinX(compassView.frame), testData.expectedOrigin.x, accuracy);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(compassView.frame), testData.expectedOrigin.y, accuracy);
    }
}

- (void)testScalebarPlacement {
    double accuracy = 0.01;
    CGFloat margin = 4.0;

    UIView *scaleBar = self.mapView.scaleBar;
    XCTAssertFalse(CGSizeEqualToSize(scaleBar.bounds.size, CGSizeZero));

    NSArray *testDataList = [self makeTestDataListWithView:scaleBar margin:margin];

    for (MGLOrnamentTestData *testData in testDataList) {
        self.mapView.scaleBarPosition = testData.position;
        self.mapView.scaleBarMargins = testData.offset;

        //invoke layout
        [self.superView setNeedsLayout];
        [self.superView layoutIfNeeded];

        XCTAssertEqualWithAccuracy(CGRectGetMinX(scaleBar.frame), testData.expectedOrigin.x, accuracy);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(scaleBar.frame), testData.expectedOrigin.y, accuracy);
    }
}

// This test checks the frames of the scalebar's subviews, based on the positions
// as above, but also with forced Right-to-Left reading, and modifying zoom levels.
- (void)testScalebarSubviewPlacement {
    double accuracy = 0.01;
    CGFloat margin = 20.0;

    MGLScaleBar *scaleBar = (MGLScaleBar*)self.mapView.scaleBar;
    XCTAssertFalse(CGSizeEqualToSize(scaleBar.bounds.size, CGSizeZero));

    for (NSInteger rtl = 0; rtl <= 1; rtl++) {
        scaleBar.testingRightToLeftOverride = @((BOOL)rtl);

        NSString *positions[] = {
            @"MGLOrnamentPositionTopLeft",
            @"MGLOrnamentPositionTopRight",
            @"MGLOrnamentPositionBottomLeft",
            @"MGLOrnamentPositionBottomRight"
        };

        for (CGFloat zoomLevel = 0; zoomLevel < 20; zoomLevel++)
        {
            self.mapView.zoomLevel = zoomLevel;
            [self.superView setNeedsLayout];
            [self.superView layoutIfNeeded];

            // Following method assumes scaleBar has an up-to-date frame, based
            // on the current zoom level. Modifying the position and margins
            // should not affect the overall size of the scalebar.

            NSArray *testDataList = [self makeTestDataListWithView:scaleBar margin:margin];

            CGSize initialSize = scaleBar.intrinsicContentSize;
            XCTAssert(CGSizeEqualToSize(initialSize, scaleBar.bounds.size));

            for (MGLOrnamentTestData *testData in testDataList) {
                self.mapView.scaleBarPosition = testData.position;
                self.mapView.scaleBarMargins = testData.offset;

                [self.superView setNeedsLayout];
                [self.superView layoutIfNeeded];

                XCTAssert(CGSizeEqualToSize(initialSize, scaleBar.bounds.size));

                NSString *activityName = [NSString stringWithFormat:
                                          @"Scalebar subview tests: RTL=%@, Zoom=%ld, POS=%@, Visible=%@",
                                          (rtl == 0 ? @"NO" : @"YES"),
                                          (long)zoomLevel,
                                          positions[testData.position],
                                          scaleBar.alpha > 0.0 ? @"YES" : @"NO"];

                [XCTContext runActivityNamed:activityName
                                       block:^(id<XCTActivity> activity) {

                    // Check the subviews
                    XCTAssertEqualWithAccuracy(CGRectGetMinX(scaleBar.frame), testData.expectedOrigin.x, accuracy);
                    XCTAssertEqualWithAccuracy(CGRectGetMinY(scaleBar.frame), testData.expectedOrigin.y, accuracy);

                    XCTAssertTrue(CGRectContainsRect(scaleBar.bounds, scaleBar.containerView.frame));
                    for (UIView *bar in scaleBar.bars) {
                        XCTAssertTrue(CGRectContainsRect(scaleBar.containerView.bounds, bar.frame));
                    }
                    for (UIView *label in scaleBar.labelViews) {
                        if (!label.isHidden) {
                            XCTAssertTrue(CGRectContainsRect(scaleBar.bounds, label.frame));
                        }
                    }
                }];
            }
        }
    }
}

- (void)testAttributionButtonPlacement {
    double accuracy = 0.01;
    CGFloat margin = 4.0;

    UIView *attributionButton = self.mapView.attributionButton;
    NSArray *testDataList = [self makeTestDataListWithView:attributionButton margin:margin];

    for (MGLOrnamentTestData *testData in testDataList) {
        self.mapView.attributionButtonPosition = testData.position;
        self.mapView.attributionButtonMargins = testData.offset;

        //invoke layout
        [self.superView setNeedsLayout];
        [self.superView layoutIfNeeded];

        XCTAssertEqualWithAccuracy(CGRectGetMinX(attributionButton.frame), testData.expectedOrigin.x, accuracy);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(attributionButton.frame), testData.expectedOrigin.y, accuracy);
    }
}

- (void)testLogoPlacement {
    double accuracy = 0.01;
    CGFloat margin = 4.0;

    UIView *logoView = self.mapView.logoView;
    NSArray *testDataList = [self makeTestDataListWithView:logoView margin:margin];

    for (MGLOrnamentTestData *testData in testDataList) {
        self.mapView.logoViewPosition = testData.position;
        self.mapView.logoViewMargins = testData.offset;

        //invoke layout
        [self.superView setNeedsLayout];
        [self.superView layoutIfNeeded];

        XCTAssertEqualWithAccuracy(CGRectGetMinX(logoView.frame), testData.expectedOrigin.x, accuracy);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(logoView.frame), testData.expectedOrigin.y, accuracy);
    }
}

@end
