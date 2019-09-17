#import "MGLRendererConfiguration.h"
#import "MGLOfflineStorage_Private.h"
#import "MGLFoundation_Private.h"

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

static NSString * const MGLCollisionBehaviorPre4_0Key = @"MGLCollisionBehaviorPre4_0";
static NSString * const MGLIdeographicFontFamilyNameKey = @"MGLIdeographicFontFamilyName";

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

- (const float)scaleFactor {
#if TARGET_OS_IPHONE
    return [UIScreen instancesRespondToSelector:@selector(nativeScale)] ? [[UIScreen mainScreen] nativeScale] : [[UIScreen mainScreen] scale];
#else
    return [NSScreen mainScreen].backingScaleFactor;
#endif
}

- (mbgl::optional<std::string>)localFontFamilyName {
    return [self _localFontFamilyNameWithPropertyDictionary:[[NSBundle mainBundle] infoDictionary]];
}

- (mbgl::optional<std::string>)_localFontFamilyNameWithPropertyDictionary:(nonnull NSDictionary *)properties {
    
    std::string systemFontFamilyName;
#if TARGET_OS_IPHONE
    systemFontFamilyName = std::string([[UIFont systemFontOfSize:0 weight:UIFontWeightRegular].familyName UTF8String]);
#else
    systemFontFamilyName = std::string([[NSFont systemFontOfSize:0 weight:NSFontWeightRegular].familyName UTF8String]);
#endif
    
    id fontFamilyName = properties[MGLIdeographicFontFamilyNameKey];
    
    if([fontFamilyName isKindOfClass:[NSNumber class]] && ![fontFamilyName boolValue])
    {
        return mbgl::optional<std::string>();
    }
    else if([fontFamilyName isKindOfClass:[NSString class]])
    {
        BOOL isValidFont = NO;
#if TARGET_OS_IPHONE
        if([[UIFont familyNames] containsObject:fontFamilyName]){
            isValidFont = YES;
        }
#else
        if([[[NSFontManager sharedFontManager] availableFontFamilies] containsObject:fontFamilyName]){
            isValidFont = YES;
        }
#endif
        return (fontFamilyName && isValidFont) ? std::string([fontFamilyName UTF8String]) : systemFontFamilyName;
    }
    // Ability to specify an array of fonts for fallbacks for `localIdeographicFontFamily`
    else if ([fontFamilyName isKindOfClass:[NSArray class]]){
        for(NSString *name in fontFamilyName){
#if TARGET_OS_IPHONE
            if([[UIFont familyNames] containsObject:name]){
                return std::string([name UTF8String]);
            }
#else
            if([[[NSFontManager sharedFontManager] availableFontFamilies] containsObject:name]){
                return std::string([name UTF8String]);
            }
#endif
        }
    }
    return systemFontFamilyName;
}

@end
