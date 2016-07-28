#import "MGLSource.h"

@interface MGLGeoJSONSource : MGLSource

- (instancetype)initWithSourceID:(NSString *)sourceID url:(NSURL *)url NS_DESIGNATED_INITIALIZER;

- (NSString *)urlString;

@end
