#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>
#import "MGLRendererConfiguration.h"

static NSString * const MGLRendererConfigurationTests_collisionBehaviorKey = @"MGLCollisionBehaviorPre4_0";

@interface MGLRendererConfiguration (Tests)
- (instancetype)initWithPropertyDictionary:(NSDictionary*)bundle;
@end


@interface MGLRendererConfigurationTests : XCTestCase
@end

@implementation MGLRendererConfigurationTests
- (void)setUp {
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:MGLRendererConfigurationTests_collisionBehaviorKey];
}

- (void)tearDown {
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:MGLRendererConfigurationTests_collisionBehaviorKey];
}

// Emulate what would happen with an Info.plist.
- (void)testSettingMGLCollisionBehaviorPre40WithEmptyDictionary
{
    MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] initWithPropertyDictionary:@{}];
    XCTAssertFalse([config perSourceCollisions]);
}

- (void)testSettingMGLCollisionBehaviorPre40WithYESDictionary
{
    MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] initWithPropertyDictionary:@{MGLRendererConfigurationTests_collisionBehaviorKey:@(NO)}];
    XCTAssertFalse([config perSourceCollisions]);
}

- (void)testSettingMGLCollisionBehaviorPre40WithNODictionary
{
    MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] initWithPropertyDictionary:@{MGLRendererConfigurationTests_collisionBehaviorKey:@(YES)}];
    XCTAssert([config perSourceCollisions]);
}

- (void)testSettingMGLCollisionBehaviorPre4_0InNSUserDefaults {
    
    __unsafe_unretained MGLRendererConfiguration *unsafeConfig1 = nil;
    
    {
        XCTAssertNil([[NSUserDefaults standardUserDefaults] objectForKey:MGLRendererConfigurationTests_collisionBehaviorKey]);
        MGLRendererConfiguration *config = [MGLRendererConfiguration currentConfiguration];
        XCTAssertFalse([config perSourceCollisions]);
        unsafeConfig1 = config;
    }
    
    __unsafe_unretained MGLRendererConfiguration *unsafeConfig2 = nil;
    [[NSUserDefaults standardUserDefaults] setObject:@(NO) forKey:MGLRendererConfigurationTests_collisionBehaviorKey];
    {
        XCTAssertNotNil([[NSUserDefaults standardUserDefaults] objectForKey:MGLRendererConfigurationTests_collisionBehaviorKey]);
        MGLRendererConfiguration *config = [MGLRendererConfiguration currentConfiguration];
        XCTAssert(config != unsafeConfig1);
        XCTAssertFalse([config perSourceCollisions]);
        unsafeConfig2 = config;
    }
    
    [[NSUserDefaults standardUserDefaults] setObject:@(YES) forKey:MGLRendererConfigurationTests_collisionBehaviorKey];
    {
        XCTAssertNotNil([[NSUserDefaults standardUserDefaults] objectForKey:MGLRendererConfigurationTests_collisionBehaviorKey]);
        MGLRendererConfiguration *config = [MGLRendererConfiguration currentConfiguration];
        XCTAssert(config != unsafeConfig1);
        XCTAssert(config != unsafeConfig2);
        XCTAssert([config perSourceCollisions]);
    }
}

@end
