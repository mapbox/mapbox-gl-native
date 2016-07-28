#import "MGLSource.h"

@interface MGLGeoJSONSource : MGLSource

@property (nonatomic, readonly, copy) NSString *data;

- (instancetype)initWithSourceID:(NSString *)sourceID url:(NSURL *)url NS_DESIGNATED_INITIALIZER;

@end
