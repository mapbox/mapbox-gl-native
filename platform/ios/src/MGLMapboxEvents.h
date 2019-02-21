#import <Foundation/Foundation.h>
#import "MMEEventsManager.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLMapboxEvents : NSObject 

+ (nullable instancetype)sharedInstance;

+ (void)setupWithAccessToken:(NSString *)accessToken;
+ (void)pushTurnstileEvent;
+ (void)pushEvent:(NSString *)event withAttributes:(MMEMapboxEventAttributes *)attributeDictionary;
+ (void)flush;
+ (void)ensureMetricsOptoutExists;

@end

NS_ASSUME_NONNULL_END
