#import "MGLRendererConfiguration.h"
#import "MGLOfflineStorage_Private.h"
#import "MGLFoundation_Private.h"

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif


@implementation MGLRendererConfiguration

+ (instancetype)currentConfiguration {
    return [[self alloc] init];
}

- (instancetype)init {
    self = [super init];
    
    if (self) {
        NSNumber *boolWrapper = [[[NSBundle mainBundle] infoDictionary] valueForKey:@"MGLCollisionBehaviorPre4_0"];
        [[NSUserDefaults standardUserDefaults] setObject:boolWrapper forKey:@"MGLCollisionBehaviorPre4_0"];
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
    NSNumber *boolWrapper = MGL_OBJC_DYNAMIC_CAST([[NSUserDefaults standardUserDefaults] objectForKey:@"MGLCollisionBehaviorPre4_0"], NSNumber);
    return boolWrapper.boolValue;
}

@end
