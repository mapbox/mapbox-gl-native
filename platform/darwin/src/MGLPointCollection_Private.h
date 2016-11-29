#import "MGLPointCollection.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLPointCollection (Private)

- (instancetype)initWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count;

@end

NS_ASSUME_NONNULL_END
