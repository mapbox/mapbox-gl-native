#import "MGLSource.h"

@interface MGLGeoJSONSource : MGLSource

@property (nonatomic, readonly, copy) NSString *data;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url NS_DESIGNATED_INITIALIZER;

@end
