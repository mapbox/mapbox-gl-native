#import "MGLRendererConfiguration.h"
#import "MGLOfflineStorage_Private.h"
#import "MGLFoundation_Private.h"

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

static NSString * const MGLCollisionBehaviorPre4_0Key = @"MGLCollisionBehaviorPre4_0";

@interface MGLRendererConfiguration ()
@property (nonatomic, readwrite) BOOL perSourceCollisions;
@end


@implementation MGLRendererConfiguration

+ (instancetype)currentConfiguration {
    return [[self alloc] init];
}

- (instancetype)init {
    return [self initWithPropertyDictionary:[[NSBundle mainBundle] infoDictionary]];
}

- (instancetype)initWithPropertyDictionary:(nonnull NSDictionary *)properties {
    self = [super init];
    
    if (self) {
        // Set the collision behaviour. A value set in `NSUserDefaults.standardUserDefaults`
        // should override anything in the application's info.plist
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

        if ([defaults objectForKey:MGLCollisionBehaviorPre4_0Key]) {
            _perSourceCollisions = [defaults boolForKey:MGLCollisionBehaviorPre4_0Key];
        }
        else {
            id collisionBehaviourValue = properties[MGLCollisionBehaviorPre4_0Key];
            
            NSNumber *collisionBehaviourNumber = MGL_OBJC_DYNAMIC_CAST(collisionBehaviourValue, NSNumber);
            
            if (collisionBehaviourNumber) {
                _perSourceCollisions = collisionBehaviourNumber.boolValue;
            } else {
                // Also support NSString to correspond with the behavior of `-[NSUserDefaults boolForKey:]`
                NSString *collisionBehaviourString = MGL_OBJC_DYNAMIC_CAST(collisionBehaviourValue, NSString);

                if (collisionBehaviourString) {
                    _perSourceCollisions = collisionBehaviourString.boolValue;
                }
            }
        }
    }
    
    return self;
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

- (std::string)localFontFamilyName {
    NSString *fontFamilyName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MGLIdeographicFontFamilyName"];

    return fontFamilyName ? std::string([fontFamilyName UTF8String]) : std::string("PingFang");
}

@end
