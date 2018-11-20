#import "MGLRendererConfiguration.h"
#import "MGLOfflineStorage_Private.h"
#import "MGLFoundation_Private.h"

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

static NSString * const MGLCollisionBehaviorPre4_0Key = @"MGLCollisionBehaviorPre4_0";

@implementation MGLRendererConfiguration

+ (instancetype)currentConfiguration {
    return [[self alloc] init];
}

- (instancetype)init {
    return [self initWithPropertyDictionary:[[NSBundle mainBundle] infoDictionary]];
}

- (instancetype)initWithPropertyDictionary:(NSDictionary *)properties {
    
    if (self) {
        
        // Set the collision behaviour

        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

        // Only use the behavior from the application's info.plist, if we don't already
        // have the behavior specified in `NSUserDefaults.standardUserDefaults`
        if (![defaults objectForKey:MGLCollisionBehaviorPre4_0Key]) {            
            NSNumber *bundleNumber = MGL_OBJC_DYNAMIC_CAST(properties[MGLCollisionBehaviorPre4_0Key], NSNumber);

            // Set or clear
            [defaults setObject:bundleNumber forKey:MGLCollisionBehaviorPre4_0Key];
        }
    }
    
    return self;
}

- (mbgl::DefaultFileSource *)fileSource {
    return [MGLOfflineStorage sharedOfflineStorage].mbglFileSource;
}

- (mbgl::GLContextMode)contextMode {
    return mbgl::GLContextMode::Unique;
}

- (const float)scaleFactor {
#if TARGET_OS_IPHONE
    return [UIScreen instancesRespondToSelector:@selector(nativeScale)] ? [[UIScreen mainScreen] nativeScale] : [[UIScreen mainScreen] scale];
#else
    return [NSScreen mainScreen].backingScaleFactor;
#endif
}

- (mbgl::optional<std::string>)cacheDir {
    return mbgl::optional<std::string>();
}

- (mbgl::optional<std::string>)localFontFamilyName {
    NSString *fontFamilyName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MGLIdeographicFontFamilyName"];

    return fontFamilyName ? std::string([fontFamilyName UTF8String]) : mbgl::optional<std::string>();
}

- (BOOL)perSourceCollisions {
    return [[NSUserDefaults standardUserDefaults] boolForKey:MGLCollisionBehaviorPre4_0Key];
}

@end
