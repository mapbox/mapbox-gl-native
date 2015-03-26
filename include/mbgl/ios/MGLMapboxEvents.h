#import <Foundation/Foundation.h>

static NSString *const MGLMapboxEventsUserAgent = @"MapboxEventsiOS/1.0";
static NSString *const MGLMapboxEventsAPIBase = @"https://api.tiles.mapbox.com";

@interface MGLMapboxEvents : NSObject

@property (atomic) NSString *token;
@property (atomic) NSString *appName;
@property (atomic) NSString *appVersion;

+ (instancetype)sharedManager;

- (void) pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary;

- (void) flush;

@end
