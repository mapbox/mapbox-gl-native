#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>
#import "MGLRendererConfiguration.h"

static NSString * const MGLRendererConfigurationTests_collisionBehaviorKey = @"MGLCollisionBehaviorPre4_0";

@interface MGLRendererConfiguration (Tests)
- (instancetype)initWithPropertyDictionary:(nonnull NSDictionary*)bundle;
- (mbgl::optional<std::string>)_localFontFamilyNameWithPropertyDictionary:(nonnull NSDictionary *)properties;
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

- (void)testDefaultLocalFontFamilyName {
    
    MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] init];
    std::string localFontFamilyName = config.localFontFamilyName.value();
    
    std::string systemFontFamilyName;
#if TARGET_OS_IPHONE
    systemFontFamilyName = std::string([[UIFont systemFontOfSize:0 weight:UIFontWeightRegular].familyName UTF8String]);
#else
    systemFontFamilyName = std::string([[NSFont systemFontOfSize:0 weight:NSFontWeightRegular].familyName UTF8String]);
#endif
    
    XCTAssertEqual(localFontFamilyName, systemFontFamilyName, @"Default local font family name should match default system font");
}

- (void)testSettingMGLIdeographicFontFamilyNameWithPlistValue {
    
    MGLRendererConfiguration *config = [[MGLRendererConfiguration alloc] init];
    NSDictionary *dic;
    
    // `MGLIdeographicFontFamilyName` set to bool value `YES`
    {
        dic = @{@"MGLIdeographicFontFamilyName": @(YES)};
        std::string localFontFamilyName = ([config _localFontFamilyNameWithPropertyDictionary:dic]).value();
        
        std::string systemFontFamilyName;
#if TARGET_OS_IPHONE
        systemFontFamilyName = std::string([[UIFont systemFontOfSize:0 weight:UIFontWeightRegular].familyName UTF8String]);
#else
        systemFontFamilyName = std::string([[NSFont systemFontOfSize:0 weight:NSFontWeightRegular].familyName UTF8String]);
#endif
        XCTAssertEqual(localFontFamilyName, systemFontFamilyName, @"Local font family name should match default system font name when setting `YES`");
    }
    
    // `MGLIdeographicFontFamilyName` set to bool value `NO`
    {
        dic = @{@"MGLIdeographicFontFamilyName": @(NO)};
        mbgl::optional<std::string> localFontFamilyName = [config _localFontFamilyNameWithPropertyDictionary:dic];
        XCTAssertFalse(localFontFamilyName.has_value(), @"Client rendering font should use remote font when setting `NO`");
    }
    
    // `MGLIdeographicFontFamilyName` set to a valid font string value
    {
        dic = @{@"MGLIdeographicFontFamilyName": @"PingFang TC"};
        std::string localFontFamilyName = ([config _localFontFamilyNameWithPropertyDictionary:dic]).value();
        std::string targetFontFamilyName = std::string([@"PingFang TC" UTF8String]);
        XCTAssertEqual(localFontFamilyName, targetFontFamilyName, @"Local font family name should match a custom valid font name");
    }
    
    // `MGLIdeographicFontFamilyName` set to an invalid font string value
    {
        dic = @{@"MGLIdeographicFontFamilyName": @"test font"};
        std::string localFontFamilyName = ([config _localFontFamilyNameWithPropertyDictionary:dic]).value();
        
        std::string systemFontFamilyName;
#if TARGET_OS_IPHONE
        systemFontFamilyName = std::string([[UIFont systemFontOfSize:0 weight:UIFontWeightRegular].familyName UTF8String]);
#else
        systemFontFamilyName = std::string([[NSFont systemFontOfSize:0 weight:NSFontWeightRegular].familyName UTF8String]);
#endif
        XCTAssertEqual(localFontFamilyName, systemFontFamilyName, @"Local font family name should match default system font name when setting an invalid font string");
    }
    
    // `MGLIdeographicFontFamilyName` set to a valid font family names array value
    {
        dic = @{@"MGLIdeographicFontFamilyName": @[@"test font 1", @"PingFang TC", @"test font 2"]};
        std::string localFontFamilyName = ([config _localFontFamilyNameWithPropertyDictionary:dic]).value();
        std::string targetFontFamilyName = std::string([@"PingFang TC" UTF8String]);
        XCTAssertEqual(localFontFamilyName, targetFontFamilyName, @"Local font family name should match a custom valid font name in a font family names array");
    }
    
    // `MGLIdeographicFontFamilyName` set to an invalid font family names array value
    {
        dic = @{@"MGLIdeographicFontFamilyName": @[@"test font 1", @"test font 2", @"test font 3"]};
        std::string localFontFamilyName = ([config _localFontFamilyNameWithPropertyDictionary:dic]).value();
        
        std::string systemFontFamilyName;
#if TARGET_OS_IPHONE
        systemFontFamilyName = std::string([[UIFont systemFontOfSize:0 weight:UIFontWeightRegular].familyName UTF8String]);
#else
        systemFontFamilyName = std::string([[NSFont systemFontOfSize:0 weight:NSFontWeightRegular].familyName UTF8String]);
#endif
        XCTAssertEqual(localFontFamilyName, systemFontFamilyName, @"Local font family name should match default system font name when setting an invalid font family names array");
    }
    
    // `MGLIdeographicFontFamilyName` set to an invalid value type: NSDictionary, NSNumber, NSData, etc.
    {
        dic = @{@"MGLIdeographicFontFamilyName": [@"test font 1" dataUsingEncoding:NSUTF8StringEncoding]};
        std::string localFontFamilyName = ([config _localFontFamilyNameWithPropertyDictionary:dic]).value();
        
        std::string systemFontFamilyName;
#if TARGET_OS_IPHONE
        systemFontFamilyName = std::string([[UIFont systemFontOfSize:0 weight:UIFontWeightRegular].familyName UTF8String]);
#else
        systemFontFamilyName = std::string([[NSFont systemFontOfSize:0 weight:NSFontWeightRegular].familyName UTF8String]);
#endif
        XCTAssertEqual(localFontFamilyName, systemFontFamilyName, @"Local font family name should match default system font name when setting an invalid value type");
    }
}



@end
