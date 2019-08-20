#import <Foundation/Foundation.h>
#import "MMEEventsManager.h"

NS_ASSUME_NONNULL_BEGIN

/// NSUserDefaults key that controls telemetry user opt-out status
FOUNDATION_EXTERN NSString * const MGLMapboxMetricsEnabledKey;

@interface MGLMapboxEvents : NSObject 

+ (nullable instancetype)sharedInstance;

+ (void)setupWithAccessToken:(NSString *)accessToken;
+ (void)pushTurnstileEvent;
+ (void)pushEvent:(NSString *)event withAttributes:(MMEMapboxEventAttributes *)attributeDictionary;
+ (void)flush;
+ (void)ensureMetricsOptoutExists;

@end

NS_ASSUME_NONNULL_END
