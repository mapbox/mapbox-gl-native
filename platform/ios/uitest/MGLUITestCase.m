#import "MGLUITestCase.h"

static NSString * const MGLUITestsBogusAccessToken = @"sk.feedCafeDeadBeef.BadeBede";

@implementation MGLUITestCase

- (void)setUp {
    [super setUp];
    
    self.continueAfterFailure = NO;
    [XCUIDevice sharedDevice].orientation = UIDeviceOrientationPortrait;
    
    // Launch the application, bypassing the access token prompt.
    XCUIApplication *app = [[XCUIApplication alloc] init];
    NSMutableDictionary <NSString *, NSString *> *environment = app.launchEnvironment.mutableCopy;
    environment[@"MAPBOX_ACCESS_TOKEN"] = MGLUITestsBogusAccessToken;
    environment[@"MAPBOX_DISABLE_ANIMATIONS"] = @"1";
    environment[@"MAPBOX_CLEAN_START"] = @"1";
    app.launchEnvironment = environment;
    [app launch];
}

@end
