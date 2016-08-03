#import <Mapbox/Mapbox.h>

@interface MGLRasterSource : MGLSource

@property (nonatomic, readonly, copy) NSURL *URL;
@property (nonatomic, readonly, assign) CGFloat tileSize;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url tileSize:(CGFloat)tileSize;

@end
