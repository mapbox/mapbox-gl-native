#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>
#import "MGLRendererConfiguration.h"

static NSString * const MGLRendererConfigurationTests_collisionBehaviorKey = @"MGLCollisionBehaviorPre4_0";

@interface MGLRendererConfiguration (Tests)
- (instancetype)initWithPropertyDictionary:(nonnull NSDictionary*)bundle;
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
    XCTAssertFalse(config.perSourceCollisions);
}

- (void)testSettingMGLCollisionBehaviorPre40WithYESDictionary
{
    MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] initWithPropertyDictionary:@{MGLRendererConfigurationTests_collisionBehaviorKey:@(NO)}];
    XCTAssertFalse(config.perSourceCollisions);
}

- (void)testSettingMGLCollisionBehaviorPre40WithNODictionary
{
    MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] initWithPropertyDictionary:@{MGLRendererConfigurationTests_collisionBehaviorKey:@(YES)}];
    XCTAssert(config.perSourceCollisions);
}

- (void)testSettingMGLCollisionBehaviorPre40InNSUserDefaults {
    {
        XCTAssertNil([[NSUserDefaults standardUserDefaults] objectForKey:MGLRendererConfigurationTests_collisionBehaviorKey]);
        MGLRendererConfiguration *config = [MGLRendererConfiguration currentConfiguration];
        XCTAssertFalse(config.perSourceCollisions);
    }
    
    [[NSUserDefaults standardUserDefaults] setObject:@(NO) forKey:MGLRendererConfigurationTests_collisionBehaviorKey];
    {
        XCTAssertNotNil([[NSUserDefaults standardUserDefaults] objectForKey:MGLRendererConfigurationTests_collisionBehaviorKey]);
        MGLRendererConfiguration *config = [MGLRendererConfiguration currentConfiguration];
        XCTAssertFalse(config.perSourceCollisions);
    }
    
    [[NSUserDefaults standardUserDefaults] setObject:@(YES) forKey:MGLRendererConfigurationTests_collisionBehaviorKey];
    {
        XCTAssertNotNil([[NSUserDefaults standardUserDefaults] objectForKey:MGLRendererConfigurationTests_collisionBehaviorKey]);
        MGLRendererConfiguration *config = [MGLRendererConfiguration currentConfiguration];
        XCTAssert(config.perSourceCollisions);
    }
}

- (void)testSettingMGLCollisionBehaviorPre40PListValueUsingString {
    // Dictionary = "NO"
    {
        MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] initWithPropertyDictionary:@{MGLRendererConfigurationTests_collisionBehaviorKey:@"NO"}];
        XCTAssertFalse(config.perSourceCollisions);
    }
    
    // Dictionary = "YES"
    {
        MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] initWithPropertyDictionary:@{MGLRendererConfigurationTests_collisionBehaviorKey:@"YES"}];
        XCTAssert(config.perSourceCollisions);
    }
}

- (void)testOverridingMGLCollisionBehaviorPre40 {

    // Dictionary = NO, NSUserDefaults = YES
    {
        [[NSUserDefaults standardUserDefaults] setObject:@(YES) forKey:MGLRendererConfigurationTests_collisionBehaviorKey];
        MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] initWithPropertyDictionary:@{MGLRendererConfigurationTests_collisionBehaviorKey:@(NO)}];
        XCTAssert(config.perSourceCollisions);
    }

    // Dictionary = YES, NSUserDefaults = NO
    {
        [[NSUserDefaults standardUserDefaults] setObject:@(NO) forKey:MGLRendererConfigurationTests_collisionBehaviorKey];
        MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] initWithPropertyDictionary:@{MGLRendererConfigurationTests_collisionBehaviorKey:@(YES)}];
        XCTAssertFalse(config.perSourceCollisions);
    }
}

- (void)testDefaultLocalFontFamilyName{
    MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] init];
    std::string localFontFamilyName = config.localFontFamilyName.value();
    
    std::string systemFontFamilyName;
#if TARGET_OS_IPHONE
    systemFontFamilyName = std::string([[UIFont systemFontOfSize:0 weight:UIFontWeightRegular].familyName UTF8String]);
#else
    systemFontFamilyName = std::string([[NSFont systemFontOfSize:0 weight:NSFontWeightRegular].familyName UTF8String]);
#endif
    
    XCTAssert(localFontFamilyName == systemFontFamilyName);
}

@end
