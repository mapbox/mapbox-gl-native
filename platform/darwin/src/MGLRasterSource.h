#import <Mapbox/Mapbox.h>

@interface MGLRasterSource : MGLSource

@property (nonatomic, readonly, copy) NSURL *url;
@property (nonatomic, readonly, assign) CGFloat tileSize;

- (instancetype)initWithSourceID:(NSString *)sourceID url:(NSURL *)url tileSize:(CGFloat)tileSize;

@end
